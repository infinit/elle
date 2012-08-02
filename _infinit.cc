#include <Infinit.hh>

#include <elle/Elle.hh>
#include <elle/utility/Parser.hh>
#include <elle/concurrency/Program.hh>

#include <nucleus/Nucleus.hh>
#include <lune/Lune.hh>
#include <agent/Agent.hh>
#include <etoile/Etoile.hh>
#include <hole/Hole.hh>
#include <horizon/Horizon.hh>

void
Infinit(elle::Natural32 argc, elle::Character* argv[])
{
  // initialize the Elle library.
  if (elle::Elle::Initialize() == elle::Status::Error)
    throw reactor::Exception(elle::concurrency::scheduler(),
                    "unable to initialize Elle");

  // set up the program.
  if (elle::concurrency::Program::Setup() == elle::Status::Error)
    throw reactor::Exception(elle::concurrency::scheduler(),
                    "unable to set up the program");

  // allocate a new parser.
  Infinit::Parser = new elle::utility::Parser(argc, argv);

  // specify a program description.
  if (Infinit::Parser->Description(Infinit::Copyright) == elle::Status::Error)
    throw reactor::Exception(elle::concurrency::scheduler(),
                    "unable to set the description");

  // register the options.
  if (Infinit::Parser->Register(
        "Help",
        'h',
        "help",
        "display the help",
        elle::utility::Parser::KindNone) == elle::Status::Error)
    throw reactor::Exception(elle::concurrency::scheduler(),
                    "unable to register the option");

  // register the option.
  if (Infinit::Parser->Register(
        "User",
        'u',
        "user",
        "specifies the name of the user",
        elle::utility::Parser::KindRequired) == elle::Status::Error)
    throw reactor::Exception(elle::concurrency::scheduler(),
                    "unable to register the option");

  // register the option.
  if (Infinit::Parser->Register(
        "Network",
        'n',
        "network",
        "specifies the name of the network",
        elle::utility::Parser::KindRequired) == elle::Status::Error)
    throw reactor::Exception(elle::concurrency::scheduler(),
                    "unable to register the option");

  // register the option.
  if (Infinit::Parser->Register(
        "Mountpoint",
        'm',
        "mountpoint",
        "specifies the mount point",
        elle::utility::Parser::KindRequired) == elle::Status::Error)
    throw reactor::Exception(elle::concurrency::scheduler(),
                    "unable to register the option");

  // parse.
  if (Infinit::Parser->Parse() == elle::Status::Error)
    throw reactor::Exception(elle::concurrency::scheduler(),
                    "unable to parse the command line");

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

      throw reactor::Exception(elle::concurrency::scheduler(),
                      "unable to retrieve the user name");
    }

  // retrieve the network name.
  if (Infinit::Parser->Value("Network",
                             Infinit::Network) == elle::Status::Error)
    {
      // display the usage.
      Infinit::Parser->Usage();

      throw reactor::Exception(elle::concurrency::scheduler(),
                      "unable to retrieve the network name");
    }

  // retrieve the mount point.
  if (Infinit::Parser->Value("Mountpoint",
                             Infinit::Mountpoint) == elle::Status::Error)
    {
      // display the usage.
      Infinit::Parser->Usage();

      throw reactor::Exception(elle::concurrency::scheduler(),
                      "unable to retrieve the mount point");
    }

  // initialize the nucleus library.
  if (nucleus::Nucleus::Initialize() == elle::Status::Error)
    throw reactor::Exception(elle::concurrency::scheduler(),
                    "unable to initialize Nucleus");

  // initialize the Lune library.
  if (lune::Lune::Initialize() == elle::Status::Error)
    throw reactor::Exception(elle::concurrency::scheduler(),
                    "unable to initialize Lune");

  // initialize Infinit.
  if (Infinit::Initialize() == elle::Status::Error)
    throw reactor::Exception(elle::concurrency::scheduler(),
                    "unable to initialize Infinit");

  // initialize the Agent library.
  if (agent::Agent::Initialize() == elle::Status::Error)
    throw reactor::Exception(elle::concurrency::scheduler(),
                    "unable to initialize Agent");

  hole::Hole::Initialize();

  // initialize the Etoile library.
  if (etoile::Etoile::Initialize() == elle::Status::Error)
    throw reactor::Exception(elle::concurrency::scheduler(),
                    "unable to initialize Etoile");

  // initialize the horizon.
  if (horizon::Horizon::Initialize() == elle::Status::Error)
    throw reactor::Exception(elle::concurrency::scheduler(),
                    "unable to initialize the horizon");

  // launch the program.
  elle::concurrency::Program::Launch();

  // delete the parser.
  delete Infinit::Parser;
  Infinit::Parser = nullptr;

  // clean the horizon.
  if (horizon::Horizon::Clean() == elle::Status::Error)
    throw reactor::Exception(elle::concurrency::scheduler(),
                    "unable to clean the horizon");

  // clean the Etoile library.
  if (etoile::Etoile::Clean() == elle::Status::Error)
    throw reactor::Exception(elle::concurrency::scheduler(),
                    "unable to clean Etoile");

  // clean the Agent library.
  if (agent::Agent::Clean() == elle::Status::Error)
    throw reactor::Exception(elle::concurrency::scheduler(),
                    "unable to clean Agent");

  // clean Hole.
  if (hole::Hole::Clean() == elle::Status::Error)
    throw reactor::Exception(elle::concurrency::scheduler(),
                    "unable to clean Hole");

  // clean Infinit.
  if (Infinit::Clean() == elle::Status::Error)
    throw reactor::Exception(elle::concurrency::scheduler(),
                    "unable to clean Infinit");

  // clean Lune
  if (lune::Lune::Clean() == elle::Status::Error)
    throw reactor::Exception(elle::concurrency::scheduler(),
                    "unable to clean Lune");

  // clean the nucleus library.
  if (nucleus::Nucleus::Clean() == elle::Status::Error)
    throw reactor::Exception(elle::concurrency::scheduler(),
                    "unable to clean Nucleus");

  // clean Elle.
  if (elle::Elle::Clean() == elle::Status::Error)
    throw reactor::Exception(elle::concurrency::scheduler(),
                    "unable to clean Elle");
}

elle::Status
Main(elle::Natural32 argc, elle::Character* argv[])
{
  try
    {
      Infinit(argc, argv);
    }
  catch (std::exception const& e)
    {
      // XXX
      show();

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

int
main(int argc, char* argv[])
{
  reactor::Scheduler& sched = elle::concurrency::scheduler();
  reactor::VThread<elle::Status> main(sched, "Infinit main",
                                      boost::bind(&Main, argc, argv));
  sched.run();
  return main.result() == elle::Status::Ok ? 0 : 1;
}
