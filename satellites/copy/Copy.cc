#include <satellites/copy/Copy.hh>

#include <elle/utility/Parser.hh>
#include <elle/concurrency/Program.hh>
#include <elle/io/Piece.hh>
#include <elle/io/Path.hh>

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

#include <elle/idiom/Close.hh>
# include <boost/foreach.hpp>
# include <limits>
#include <elle/idiom/Open.hh>

#include <iostream>
#include <fstream>

namespace satellite
{
  reactor::network::TCPSocket* Copy::socket = nullptr;
  infinit::protocol::Serializer* Copy::serializer = nullptr;
  infinit::protocol::ChanneledStream* Copy::channels = nullptr;
  etoile::portal::RPC* Copy::rpcs = nullptr;

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
      if (_clean && Copy::socket != nullptr)
        Copy::rpcs->objectdiscard(this->_id);
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
  Copy::connect()
  {
    // Load the phrase.
    lune::Phrase phrase;
    phrase.load(Infinit::Network, "portal");

    // Connect to the server.
    Copy::socket =
      new reactor::network::TCPSocket(elle::concurrency::scheduler(),
                                      elle::String("127.0.0.1"),
                                      phrase.port);
    Copy::serializer =
      new infinit::protocol::Serializer(elle::concurrency::scheduler(),
                                        *socket);
    Copy::channels =
      new infinit::protocol::ChanneledStream(elle::concurrency::scheduler(),
                                             *serializer, true);
    Copy::rpcs = new etoile::portal::RPC(*channels);

    if (!Copy::rpcs->authenticate(phrase.pass))
      throw reactor::Exception(elle::concurrency::scheduler(),
                               "unable to authenticate to Etoile");
  }

  void
  Copy::from(elle::String const& source,
             elle::String const& target)
  {
    Copy::connect();
    /* XXX
    // Resolve the path.
    etoile::path::Chemin chemin(Copy::rpcs->pathresolve(way));
    // Load the object.
    etoile::gear::Identifier identifier(Copy::rpcs->objectload(chemin));
    Ward ward(identifier);
    // Lookup the copy record.
    nucleus::neutron::Record record(
      Copy::rpcs->copylookup(identifier, subject));
    display(record);
    */
  }

  void
  Copy::to(elle::String const& source,
           elle::String const& target)
  {
    etoile::path::Slab name;
    etoile::path::Way way(etoile::path::Way(target), name);

    // Connect to Etoile.
    Copy::connect();

    // Resolve parent directory.
    etoile::path::Chemin chemin(Copy::rpcs->pathresolve(way));

    // Load parent directory.
    etoile::gear::Identifier directory(Copy::rpcs->directoryload(chemin));

    Ward ward_directory(directory);

    // Check permissions.
    nucleus::neutron::Record record(
      Copy::rpcs->accesslookup(directory, agent::Agent::Subject));

    if ((record == nucleus::neutron::Record::Null) ||
        ((record.permissions & nucleus::neutron::permissions::write) !=
         nucleus::neutron::permissions::write))
      throw std::runtime_error("the subject does not have the right to create "
                               "a file in this directory");

    // Create file.
    etoile::gear::Identifier file(Copy::rpcs->filecreate());

    Ward ward_file(file);

    // Grant permissions for the user itself.
    // Set default permissions: read and write.
    nucleus::neutron::Permissions permissions =
      nucleus::neutron::permissions::read |
      nucleus::neutron::permissions::write;

    // Set the owner permissions.
    Copy::rpcs->accessgrant(file, agent::Agent::Subject, permissions);

    // FIXME: do not re-parse the descriptor every time.
    lune::Descriptor descriptor(Infinit::Network);

    // Grant read permission for 'everybody' group.
    switch (descriptor.data().policy())
      {
      case horizon::Policy::accessible:
        {
          // grant the read permission to the 'everybody' group.
          Copy::rpcs->accessgrant(file,
                                  descriptor.meta().everybody_subject(),
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

    // Write the source file's content into the Infinit file freshly created.
    std::streamsize N = 4096;
    std::ifstream stream(source, std::ios::binary);
    nucleus::neutron::Offset offset(0);

    while (stream.good())
      {
        unsigned char buffer[N];

        stream.read((char*)buffer, N);

        elle::standalone::Region data(buffer, N);

        data.size = stream.gcount();

        Copy::rpcs->filewrite(file, offset, data);

        offset += data.size;
      }

    // Add file to parent directory.
    Copy::rpcs->directoryadd(directory, name, file);

    // Store file.
    Copy::rpcs->filestore(file);

    // Store parent directory.
    Copy::rpcs->directorystore(directory);

    // Release the identifier tracking.
    ward_file.release();
    ward_directory.release();
  }

  elle::Status          Main(elle::Natural32                    argc,
                             elle::Character*                   argv[])
  {
    Copy::Operation   operation;

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
    operation = Copy::OperationUnknown;

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
          "Source",
          's',
          "source",
          "the path to the source file i.e the file to be copied",
          elle::utility::Parser::KindRequired) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Target",
          'g',
          "target",
          "the path to the target file to be created",
          elle::utility::Parser::KindRequired) == elle::Status::Error)
      escape("unable to register the option");

    if (Infinit::Parser->Example(
          "-u fistouille -n slug --to --source ~/Downloads/foo.txt "
          "--target /foo.txt") == elle::Status::Error)
      escape("unable to register the example");

    if (Infinit::Parser->Example(
          "-u fistouille -n slug --from --source /partouze.avi "
          "--target ~/Videos/partouze.avi") == elle::Status::Error)
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
        return elle::Status::Ok;
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
      operation = Copy::OperationFrom;

    // test the option.
    if (Infinit::Parser->Test("To") == elle::Status::True)
      operation = Copy::OperationTo;

    elle::String source;
    elle::String target;

    // retrieve the path.
    if (Infinit::Parser->Value("Source",
                               source) == elle::Status::Error)
      escape("unable to retrieve the source value");

    // retrieve the path.
    if (Infinit::Parser->Value("Target",
                               target) == elle::Status::Error)
      escape("unable to retrieve the target value");

    // trigger the operation.
    switch (operation)
      {
      case Copy::OperationFrom:
        {
          Copy::from(source, target);

          break;
        }
      case Copy::OperationTo:
        {
          Copy::to(source, target);

          break;
        }
      case Copy::OperationUnknown:
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

    return elle::Status::Ok;
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
      if (satellite::Main(argc, argv) == elle::Status::Error)
        throw reactor::Exception(elle::concurrency::scheduler(),
                                 "XXX");
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
