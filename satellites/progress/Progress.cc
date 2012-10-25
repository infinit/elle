#include <satellites/progress/Progress.hh>

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

#include <agent/Agent.hh>

#include <elle/idiom/Close.hh>
# include <boost/foreach.hpp>
# include <limits>
#include <elle/idiom/Open.hh>

namespace satellite
{
  reactor::network::TCPSocket* Progress::socket = nullptr;
  infinit::protocol::Serializer* Progress::serializer = nullptr;
  infinit::protocol::ChanneledStream* Progress::channels = nullptr;
  etoile::portal::RPC* Progress::rpcs = nullptr;

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
      if (_clean && Progress::socket != nullptr)
        Progress::rpcs->objectdiscard(this->_id);
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
  Progress::connect()
  {
    // Load the phrase.
    lune::Phrase        phrase;
    phrase.load(Infinit::Network, "portal");

    // Connect to the server.
    Progress::socket =
      new reactor::network::TCPSocket(elle::concurrency::scheduler(),
                                      elle::String("127.0.0.1"),
                                      phrase.port);
    Progress::serializer =
      new infinit::protocol::Serializer(elle::concurrency::scheduler(), *socket);
    Progress::channels =
      new infinit::protocol::ChanneledStream(elle::concurrency::scheduler(),
                                             *serializer, true);
    Progress::rpcs = new etoile::portal::RPC(*channels);

    if (!Progress::rpcs->authenticate(phrase.pass))
      throw reactor::Exception(elle::concurrency::scheduler(),
                               "unable to authenticate to Etoile");
  }

  void
  Progress::display()
  {
    elle::Natural64 _size(0);
    elle::Natural64 _progress(0);

    // Connect to etoile.
    Progress::connect();

    // (1) Get the transfer size from the root directory.
    {
      // Resolve the path to the root directory.
      etoile::path::Chemin chemin(
        Progress::rpcs->pathresolve(
          etoile::path::Way(elle::system::path::separator)));

      // Load the root directory.
      etoile::gear::Identifier directory(
        Progress::rpcs->directoryload(chemin));

      Ward ward_directory(directory);

      // Then, retrieve the size of the transfer.
      nucleus::neutron::Trait size(
        Progress::rpcs->attributesget(directory,
                                      "infinit:transfer:size"));

      if (size == nucleus::neutron::Trait::Null)
        throw std::runtime_error("no transfer size attribute present");

      // Set the size variable.
      _size = boost::lexical_cast<elle::Natural64>(size.value);

      // Discard the directory since unchanged.
      Progress::rpcs->directorydiscard(directory);

      ward_directory.release();
    }

    // (2) Get the progress attribute from the specific file.
    {
      // The way to the progress-specific file.
      elle::String root(elle::String(1, elle::system::path::separator) +
                        ".progress");
      etoile::path::Way way(root);

      etoile::path::Chemin* chemin(nullptr);

      try
        {
          // Resolve the file.
          chemin = new etoile::path::Chemin(Progress::rpcs->pathresolve(way));
        }
      catch (...)
        {
          std::cout << "0" << std::endl;
          return;
        }

      // Load the progress file.
      etoile::gear::Identifier identifier(Progress::rpcs->fileload(*chemin));

      delete chemin;

      Ward ward(identifier);

      // Retrieve the progress and size attributes.
      nucleus::neutron::Trait progress(
        Progress::rpcs->attributesget(identifier,
                                      "infinit:transfer:progress"));

      if (progress == nucleus::neutron::Trait::Null)
        {
          std::cout << "0" << std::endl;
          return;
        }

      // Set the progress variable.
      _progress = boost::lexical_cast<elle::Natural64>(progress.value);

      // Discard the file.
      Progress::rpcs->filediscard(identifier);

      ward.release();
    }

    // (3) Display the progress.
    elle::Natural64 pourcentage(_progress * 100 / _size);

    std::cout << pourcentage << std::endl;
  }

  void
  main(elle::Natural32 argc,
       elle::Character* argv[])
  {
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

    // parse.
    if (Infinit::Parser->Parse() == elle::Status::Error)
      escape("unable to parse the command line");

    // test the option.
    if (Infinit::Parser->Test("Help") == elle::Status::True)
      {
        // display the usage.
        Infinit::Parser->Usage();

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

    // display the progress.
    Progress::display();

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

int
_main(elle::Natural32 argc, elle::Character* argv[])
{
  try
    {
      satellite::main(argc, argv);
    }
  catch (reactor::Exception const& e)
    {
      std::cerr << argv[0] << ": fatal error: " << e << std::endl;
      goto _error;
    }
  catch (std::exception const& e)
    {
      std::cerr << argv[0] << ": fatal error: " << e.what() << std::endl;
      goto _error;
    }
  catch (...)
    {
      std::cerr << argv[0] << ": unknown exception" << std::endl;
      goto _error;
    }

  elle::concurrency::scheduler().terminate();
  return (0);

 _error:
  elle::concurrency::scheduler().terminate();
  return (1);
}

int                     main(int                                argc,
                             char**                             argv)
{
  reactor::Scheduler& sched = elle::concurrency::scheduler();
  reactor::VThread<int> main(sched, "main",
                             boost::bind(&_main, argc, argv));
  sched.run();
  return (main.result());
}
