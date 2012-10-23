#include <satellites/transfer/Transfer.hh>

#include <elle/utility/Parser.hh>
#include <elle/concurrency/Program.hh>
#include <elle/io/Piece.hh>
#include <elle/io/Path.hh>
#include <elle/system/system.hh>

#include <reactor/network/tcp-socket.hh>

#include <protocol/Serializer.hh>

#include <etoile/gear/Identifier.hh>
#include <etoile/path/Chemin.hh>
#include <etoile/path/Way.hh>
#include <etoile/portal/Manifest.hh>

#include <nucleus/neutron/Range.hh>
#include <nucleus/neutron/Record.hh>
#include <nucleus/neutron/Subject.hh>

#include <lune/Lune.hh>
#include <lune/Phrase.hh>
#include <lune/Descriptor.hh>

#include <agent/Agent.hh>

#include <elle/log.hh>

#include <elle/idiom/Close.hh>
# include <boost/filesystem.hpp>
# include <boost/algorithm/string.hpp>
# include <limits>
#include <elle/idiom/Open.hh>

#include <iostream>
#include <fstream>

ELLE_LOG_COMPONENT("infinit.satellites.transfer.Transfer");

namespace satellite
{
  reactor::network::TCPSocket* Transfer::socket = nullptr;
  infinit::protocol::Serializer* Transfer::serializer = nullptr;
  infinit::protocol::ChanneledStream* Transfer::channels = nullptr;
  etoile::portal::RPC* Transfer::rpcs = nullptr;
  lune::Descriptor* Transfer::descriptor = nullptr;

  /// Ward helper to make sure objects are discarded on errors.
  class Ward
  {
  public:
    Ward(etoile::gear::Identifier const& id):
      _id(id),
      _clean(true)
    {}

    ~Ward()
    {
      if (_clean && Transfer::socket != nullptr)
        Transfer::rpcs->objectdiscard(this->_id);
    }

    void release()
    {
      _clean = false;
    }

  private:
    etoile::gear::Identifier _id;
    bool _clean;
  };

  void
  Transfer::connect()
  {
    // Load the phrase.
    lune::Phrase phrase;
    phrase.load(Infinit::Network, "portal");

    // Connect to the server.
    Transfer::socket =
      new reactor::network::TCPSocket(elle::concurrency::scheduler(),
                                      elle::String("127.0.0.1"),
                                      phrase.port);
    Transfer::serializer =
      new infinit::protocol::Serializer(elle::concurrency::scheduler(),
                                        *socket);
    Transfer::channels =
      new infinit::protocol::ChanneledStream(elle::concurrency::scheduler(),
                                             *serializer, true);
    Transfer::rpcs = new etoile::portal::RPC(*channels);

    if (!Transfer::rpcs->authenticate(phrase.pass))
      throw reactor::Exception(elle::concurrency::scheduler(),
                               "unable to authenticate to Etoile");
  }

  static elle::Natural64 _progress(0);
  static elle::Natural64 _size(0);

  etoile::path::Chemin
  Transfer::from_setup()
  {
    // Resolve the directory.
    etoile::path::Chemin chemin(Transfer::rpcs->pathresolve(
      etoile::path::Way(elle::system::path::separator)));

    // Load the directory.
    etoile::gear::Identifier identifier(
      Transfer::rpcs->directoryload(chemin));

    // Get the initial attribute, if present.
    nucleus::neutron::Trait progress(
      Transfer::rpcs->attributesget(identifier,
                                    "infinit:transfer:progress"));

    if (progress == nucleus::neutron::Trait::Null)
      {
        // Set the initial attribute.
        Transfer::rpcs->attributesset(identifier,
                                      "infinit:transfer:progress", "0");
        _progress = 0;
      }
    else
      {
        _progress = boost::lexical_cast<elle::Natural64>(progress.value);
      }

    // Then, retrieve the size of the transfer.
    nucleus::neutron::Trait size(
      Transfer::rpcs->attributesget(identifier,
                                    "infinit:transfer:size"));

    if (size == nucleus::neutron::Trait::Null)
      throw std::runtime_error("no transfer size attribute present");

    // Set the size variable.
    _size = boost::lexical_cast<elle::Natural64>(size.value);

    // Store the directory since some modifications may have been
    // performed.
    Transfer::rpcs->directorystore(identifier);

    return (chemin);
  }

  void
  Transfer::from_progress(elle::Natural64 increment)
  {
    // Setup the progress update and keep the chemin which is
    // not going to change.
    static etoile::path::Chemin chemin(Transfer::from_setup());
    static elle::Natural64 stale(_progress);

    // Increment the progress counter.
    _progress += increment;

    // Compute the increment in terms of pourcentage of progress.
    elle::Real difference = (_progress - stale) * 100 / _size;

    // If the difference is large enough, update the progress in the root
    // directory's attribtues.
    if ((difference > 1.0) || (_progress == _size))
      {
        // Load the root directory.
        etoile::gear::Identifier identifier(
          Transfer::rpcs->directoryload(chemin));

        Ward ward(identifier);

        elle::String string =
          boost::lexical_cast<elle::String>(_progress);

        // Update the progress attribute.
        Transfer::rpcs->attributesset(identifier,
                                      "infinit:transfer:progress",
                                      string);

        // Store the modifications.
        Transfer::rpcs->directorystore(identifier);

        ward.release();

        // Update the stale progress which now is up-to-date.
        stale = _progress;
      }
  }

  void
  Transfer::from_traverse(etoile::path::Way const& source,
                          elle::String const& target)
  {
    // Resolve the directory.
    etoile::path::Chemin chemin(Transfer::rpcs->pathresolve(source));

    // Load the directory.
    etoile::gear::Identifier directory(Transfer::rpcs->directoryload(chemin));

    Ward ward_directory(directory);

    // Consult the directory.
    nucleus::neutron::Range<nucleus::neutron::Entry> entries(
      Transfer::rpcs->directoryconsult(
        directory,
        0, std::numeric_limits<nucleus::neutron::Index>::max()));

    // Go through the entries.
    for (auto trait: entries)
      {
        etoile::path::Way _source(source.path +
                                  trait->name);

        ELLE_TRACE("source %s", _source.path.c_str());

        // Resolve the child.
        etoile::path::Chemin chemin(Transfer::rpcs->pathresolve(_source));

        // Load the child.
        etoile::gear::Identifier child(Transfer::rpcs->objectload(chemin));

        Ward ward_child(child);

        // Retrieve information on the child.
        etoile::abstract::Object abstract(
          Transfer::rpcs->objectinformation(child));

        elle::String path(target + _source.path);

        switch (abstract.genre)
          {
          case nucleus::neutron::Genre::file:
            {
              std::streamsize N = 5242880;
              std::ofstream stream(path, std::ios::binary);
              nucleus::neutron::Offset offset(0);

              ELLE_TRACE("file %s", path.c_str());

              // Copy the file.
              while (offset < abstract.size)
                {
                  elle::standalone::Region data(
                    Transfer::rpcs->fileread(child, offset, N));

                  stream.write((const char*)data.contents,
                               static_cast<std::streamsize>(data.size));

                  offset += data.size;

                  // Set the progress.
                  Transfer::from_progress(data.size);
                }

              // Make sure the right amount has been copied.
              assert(offset == abstract.size);

              stream.close();

              // Discard the child.
              Transfer::rpcs->objectdiscard(child);

              ward_child.release();

              break;
            }
          case nucleus::neutron::Genre::directory:
            {
              ELLE_TRACE("directory %s", path.c_str());

              // Create the directory.
              if (boost::filesystem::create_directory(path) == false)
                throw std::runtime_error("unable to create the directory");

              // Set the progress.
              Transfer::from_progress(1);

              // Recursively explore the Infinit network.
              Transfer::from_traverse(_source.path +
                                      elle::system::path::separator,
                                      target);

              // Discard the child.
              Transfer::rpcs->objectdiscard(child);

              ward_child.release();

              break;
            }
          case nucleus::neutron::Genre::link:
            {
              ELLE_TRACE("link %s", path.c_str());

              // Resolve the link.
              etoile::path::Way way(Transfer::rpcs->linkresolve(child));

              // Create the link.
              boost::filesystem::create_symlink(way.path, path);

              // Set the progress.
              Transfer::from_progress(way.path.length());

              // Discard the child.
              Transfer::rpcs->objectdiscard(child);

              ward_child.release();

              break;
            }
          }
      }

    // Discard the directory since no longer necessary.
    Transfer::rpcs->directorydiscard(directory);

    ward_directory.release();
  }

  void
  Transfer::from(elle::String const& target)
  {
    // Connect to Etoile.
    Transfer::connect();

    // Traverse the Infinit network from the root.
    Transfer::from_traverse(etoile::path::Way(elle::system::path::separator),
                            target);
  }

  void
  Transfer::to_update(elle::Natural64 const size)
  {
    etoile::path::Way root("/");

    // Resolve the root directory.
    etoile::path::Chemin chemin(Transfer::rpcs->pathresolve(root));

    // Load the directory.
    etoile::gear::Identifier directory(Transfer::rpcs->directoryload(chemin));

    Ward ward_directory(directory);

    // Set the attribute.
    Transfer::rpcs->attributesset(directory,
                                  "infinit:transfer:size", elle::sprint(size));

    // Store the directory.
    Transfer::rpcs->directorystore(directory);

    ward_directory.release();
  }

  void
  Transfer::to_attach(etoile::gear::Identifier& object,
                      elle::String const& path)
  {
    etoile::path::Slab name;
    etoile::path::Way way(etoile::path::Way(path), name);

    // Resolve parent directory.
    etoile::path::Chemin chemin(Transfer::rpcs->pathresolve(way));

    // Load parent directory.
    etoile::gear::Identifier directory(Transfer::rpcs->directoryload(chemin));

    Ward ward_directory(directory);

    // Retrieve the subject's permissions on the object.
    nucleus::neutron::Record record(
      Transfer::rpcs->accesslookup(directory, agent::Agent::Subject));

    // Check the record.
    if ((record == nucleus::neutron::Record::Null) ||
        ((record.permissions & nucleus::neutron::permissions::write) !=
         nucleus::neutron::permissions::write))
      throw std::runtime_error("the subject does not have the permission");

    // Grant permissions for the user itself.
    // Set default permissions: read and write.
    nucleus::neutron::Permissions permissions =
      nucleus::neutron::permissions::read |
      nucleus::neutron::permissions::write;

    // Set the owner permissions.
    Transfer::rpcs->accessgrant(object, agent::Agent::Subject, permissions);

    // Grant read permission for 'everybody' group.
    switch (Transfer::descriptor->data().policy())
      {
      case horizon::Policy::accessible:
        {
          // grant the read permission to the 'everybody' group.
          Transfer::rpcs->accessgrant(
            object,
            Transfer::descriptor->meta().everybody_subject(),
            nucleus::neutron::permissions::read);

          break;
        }
      case horizon::Policy::editable:
        {
          // XXX
          assert(false && "not yet supported");

          break;
        }
      case horizon::Policy::confidential:
        {
          // Nothing else to do in this case, the file system object
          // remains private to its owner.

          break;
        }
      default:
        {
          throw std::runtime_error("invalid policy");
        }
      }

    // Add object to parent directory.
    Transfer::rpcs->directoryadd(directory, name, object);

    // Store parent directory.
    Transfer::rpcs->directorystore(directory);

    // Release the identifier tracking.
    ward_directory.release();
  }

  elle::Natural64
  Transfer::to_create(elle::String const& source,
                      elle::String const& target)
  {
    // Create file.
    etoile::gear::Identifier file(Transfer::rpcs->filecreate());

    Ward ward_file(file);

    // Attach the file to the hierarchy.
    Transfer::to_attach(file, target);

    // Write the source file's content into the Infinit file freshly created.
    std::streamsize N = 5242880;
    std::ifstream stream(source, std::ios::binary);
    nucleus::neutron::Offset offset(0);
    unsigned char* buffer = new unsigned char[N];

    while (stream.good())
      {
        stream.read((char*)buffer, N);

        elle::standalone::Region data(buffer, N);

        data.size = stream.gcount();

        Transfer::rpcs->filewrite(file, offset, data);

        offset += data.size;
      }

    stream.close();

    delete[] buffer;

    // Store file.
    Transfer::rpcs->filestore(file);

    // Release the identifier tracking.
    ward_file.release();

    // Return the number of bytes composing the file having been copied.
    return (static_cast<elle::Natural64>(offset));
  }

  elle::Natural64
  Transfer::to_dig(elle::String const& path)
  {
    // Create directory.
    etoile::gear::Identifier subdirectory(Transfer::rpcs->directorycreate());

    Ward ward_subdirectory(subdirectory);

    // Attach the directory to the hierarchy.
    Transfer::to_attach(subdirectory, path);

    // Store subdirectory.
    Transfer::rpcs->directorystore(subdirectory);

    // Release the identifier tracking.
    ward_subdirectory.release();

    // We consider that the directories do not account for the actual data but
    // for a single byte.
    return (1);
  }

  elle::Natural64
  Transfer::to_symlink(elle::String const& source,
                       elle::String const& target)
  {
    // Create symlink.
    etoile::gear::Identifier link(Transfer::rpcs->linkcreate());

    Ward ward_link(link);

    // Attach the link to the hierarchy.
    Transfer::to_attach(link, target);

    etoile::path::Way way(boost::filesystem::read_symlink(source).string());

    // bind the link.
    Transfer::rpcs->linkbind(link, way);

    // Store link.
    Transfer::rpcs->linkstore(link);

    // Release the identifier tracking.
    ward_link.release();

    return (way.path.length());
  }

  void
  Transfer::to(elle::String const& source)
  {
    elle::Natural64 size(0);

    // Connect to Etoile.
    Transfer::connect();

    boost::filesystem::path path(source);

    if (boost::filesystem::is_symlink(path) == true)
      {
        // Transfor a single link.
        elle::String root(path.parent_path().string());
        elle::String base(path.string().substr(root.length()));

        ELLE_TRACE("root %s", root.c_str());
        ELLE_TRACE("link %s", base.c_str());

        size += Transfer::to_symlink(source, base);
      }
    else if (boost::filesystem::is_directory(path) == true)
      {
        // Transfer a whole directory and its content.
        elle::String root(path.parent_path().string());
        elle::String base(path.string().substr(root.length()));

        ELLE_TRACE("root %s", root.c_str());
        ELLE_TRACE("base %s", base.c_str());

        boost::filesystem::recursive_directory_iterator iterator(source);
        boost::filesystem::recursive_directory_iterator end;

        size += Transfer::to_dig(base);

        for (; iterator != end; ++iterator)
          {
            ELLE_TRACE("path %s", iterator->path().string().c_str());

            if (boost::filesystem::is_symlink(iterator->path()) == true)
              {
                elle::String link(
                  iterator->path().string().substr(root.length()));

                ELLE_TRACE("link %s", link.c_str());

                size += Transfer::to_symlink(iterator->path().string(), link);
              }
            else if (boost::filesystem::is_regular_file(
                       iterator->path()) == true)
              {
                elle::String file(
                  iterator->path().string().substr(root.length()));

                ELLE_TRACE("file %s", file.c_str());

                size += Transfer::to_create(iterator->path().string(), file);
              }
            else if (boost::filesystem::is_directory(iterator->path()) == true)
              {
                elle::String directory(
                  iterator->path().string().substr(root.length()));

                ELLE_TRACE("directory %s", directory.c_str());

                size += Transfer::to_dig(directory);
              }
            else
              throw std::runtime_error("unknown object type");
          }
      }
    else if (boost::filesystem::is_regular_file(path) == true)
      {
        // Transfor a single file.
        elle::String root(path.parent_path().string());
        elle::String base(path.string().substr(root.length()));

        ELLE_TRACE("root %s", root.c_str());
        ELLE_TRACE("file %s", base.c_str());

        size += Transfer::to_create(source, base);
      }
    else
      throw std::runtime_error("unknown object type");

    Transfer::to_update(size);
  }

  void
  main(elle::Natural32 argc,
       elle::Character* argv[])
  {
    Transfer::Operation operation;

    // XXX Infinit::Parser is not deleted in case of errors

    // set up the program.
    if (elle::concurrency::Program::Setup() == elle::Status::Error)
      escape("unable to set up the program");

    // initialize the Lune library.
    if (lune::Lune::Initialize() == elle::Status::Error)
      escape("unable to initialize Lune");

    // initialize Infinit.
    if (Infinit::Initialize() == elle::Status::Error)
      escape("unable to initialize Infinit");

    // initialize the operation.
    operation = Transfer::OperationUnknown;

    // allocate a new parser.
    Infinit::Parser = new elle::utility::Parser(argc, argv);

    // specify a program description.
    if (Infinit::Parser->Description(Infinit::Copyright) == elle::Status::Error)
      escape("unable to set the description");

    // register the options.
    if (Infinit::Parser->Register(
          "Help",
          'h',
          "help",
          "display the help",
          elle::utility::Parser::KindNone) == elle::Status::Error)
      escape("unable to register the option");

    // register the option.
    if (Infinit::Parser->Register(
          "User",
          'u',
          "user",
          "specifies the name of the user",
          elle::utility::Parser::KindRequired) == elle::Status::Error)
      escape("unable to register the option");

    // register the option.
    if (Infinit::Parser->Register(
          "Network",
          'n',
          "network",
          "specifies the name of the network",
          elle::utility::Parser::KindRequired) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "From",
          'f',
          "from",
          "specifies that the file is copied from the Infinit network to "
          "the local file system",
          elle::utility::Parser::KindNone) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "To",
          't',
          "too",
          "specifies that the file is copied from the local file system to "
          "the Infinit network",
          elle::utility::Parser::KindNone) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Path",
          'p',
          "path",
          "the directory path where the data must be copied from/to",
          elle::utility::Parser::KindRequired) == elle::Status::Error)
      escape("unable to register the option");

    if (Infinit::Parser->Example(
          "-u fistouille -n slug --to --path ~/Downloads/") ==
        elle::Status::Error)
      escape("unable to register the example");

    if (Infinit::Parser->Example(
          "-u fistouille -n slug --from --path /tmp/XXX/") ==
        elle::Status::Error)
      escape("unable to register the example");

    // parse.
    if (Infinit::Parser->Parse() == elle::Status::Error)
      escape("unable to parse the command line");

    // test the option.
    if (Infinit::Parser->Test("Help") == elle::Status::True)
      {
        // display the usage.
        Infinit::Parser->Usage();

        // quit.
        return;
      }

    // retrieve the user name.
    if (Infinit::Parser->Value("User",
                               Infinit::User) == elle::Status::Error)
      {
        // display the usage.
        Infinit::Parser->Usage();

        escape("unable to retrieve the user name");
      }

    // retrieve the network name.
    if (Infinit::Parser->Value("Network",
                               Infinit::Network) == elle::Status::Error)
      {
        // display the usage.
        Infinit::Parser->Usage();

        escape("unable to retrieve the network name");
      }

    // initialize the Agent library.
    if (agent::Agent::Initialize() == elle::Status::Error)
      escape("unable to initialize Agent");

    // check the mutually exclusive options.
    if ((Infinit::Parser->Test("From") == elle::Status::True) &&
        (Infinit::Parser->Test("To") == elle::Status::True))
      {
        // display the usage.
        Infinit::Parser->Usage();

        escape("the from and to options are mutually exclusive");
      }

    // test the option.
    if (Infinit::Parser->Test("From") == elle::Status::True)
      operation = Transfer::OperationFrom;

    // test the option.
    if (Infinit::Parser->Test("To") == elle::Status::True)
      operation = Transfer::OperationTo;

    // FIXME: do not re-parse the descriptor every time.
    Transfer::descriptor = new lune::Descriptor(Infinit::Network);

    elle::String path;

    // retrieve the path.
    if (Infinit::Parser->Value("Path",
                               path) == elle::Status::Error)
      escape("unable to retrieve the path value");

    path =
      boost::algorithm::trim_right_copy_if(
        boost::filesystem::absolute(path).string(),
        boost::is_any_of("/"));

    // trigger the operation.
    switch (operation)
      {
      case Transfer::OperationFrom:
        {
          Transfer::from(path);

          break;
        }
      case Transfer::OperationTo:
        {
          Transfer::to(path);

          break;
        }
      case Transfer::OperationUnknown:
      default:
        {
          // display the usage.
          Infinit::Parser->Usage();

          escape("please specify an operation to perform");
        }
      }

    // delete the parser.
    delete Infinit::Parser;
    Infinit::Parser = nullptr;

    // clean the Agent library.
    if (agent::Agent::Clean() == elle::Status::Error)
      escape("unable to clean Agent");

    // clean Infinit.
    if (Infinit::Clean() == elle::Status::Error)
      escape("unable to clean Infinit");

    // clean Lune
    if (lune::Lune::Clean() == elle::Status::Error)
      escape("unable to clean Lune");
  }

}

//
// ---------- main ------------------------------------------------------------
//

elle::Status
_main(elle::Natural32 argc, elle::Character* argv[])
{
  try
    {
      satellite::main(argc, argv);
    }
  catch (std::exception const& e)
    {
      std::cerr << argv[0] << ": fatal error: " << e.what() << std::endl;
      if (reactor::Exception const* re =
          dynamic_cast<reactor::Exception const*>(&e))
        std::cerr << re->backtrace() << std::endl;

      elle::concurrency::scheduler().terminate();
      return elle::Status::Error;
    }
  elle::concurrency::scheduler().terminate();
  return elle::Status::Ok;
}

///
/// this is the program entry point.
///
int                     main(int                                argc,
                             char**                             argv)
{
  reactor::Scheduler& sched = elle::concurrency::scheduler();
  reactor::VThread<elle::Status> main(sched, "main",
                                      boost::bind(&_main, argc, argv));
  sched.run();
  return main.result() == elle::Status::Ok ? 0 : 1;
}
