#include <Infinit.hh>

#include <elle/Elle.hh>
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
  if (elle::Elle::Initialize() == elle::StatusError)
    throw std::runtime_error("unable to initialize Elle");

  // set up the program.
  if (elle::Program::Setup() == elle::StatusError)
    throw std::runtime_error("unable to set up the program");

  // allocate a new parser.
  Infinit::Parser = new elle::Parser(argc, argv);

  // specify a program description.
  if (Infinit::Parser->Description(Infinit::Copyright) == elle::StatusError)
    throw std::runtime_error("unable to set the description");

  // register the options.
  if (Infinit::Parser->Register(
        "Help",
        'h',
        "help",
        "display the help",
        elle::Parser::KindNone) == elle::StatusError)
    throw std::runtime_error("unable to register the option");

  // register the option.
  if (Infinit::Parser->Register(
        "Network",
        'n',
        "network",
        "specifies the name of the network",
        elle::Parser::KindRequired) == elle::StatusError)
    throw std::runtime_error("unable to register the option");

  // register the option.
  if (Infinit::Parser->Register(
        "Mountpoint",
        'm',
        "mountpoint",
        "specifies the mount point",
        elle::Parser::KindRequired) == elle::StatusError)
    throw std::runtime_error("unable to register the option");

  // parse.
  if (Infinit::Parser->Parse() == elle::StatusError)
    throw std::runtime_error("unable to parse the command line");

  // test the option.
  if (Infinit::Parser->Test("Help") == elle::StatusTrue)
    {
      // display the usage.
      Infinit::Parser->Usage();

      // quit.
      return;
    }

  // retrieve the user name.
#if defined(INFINIT_LINUX) || defined(INFINIT_MACOSX)
  {
    struct ::passwd*    pw;

    // retrieve the current password
    if ((pw = ::getpwuid(geteuid())) == NULL)
      throw std::runtime_error("unable to retrieve the current user's password structure");

    // assign the username.
    Infinit::User.assign(pw->pw_name);
    // XXX[this will take the current user and therefore displays:
    //     `enter your passphrase ...`]
  }
#elif defined(INFINIT_WINDOWS)
  {
    char                username[1024];
    DWORD               length = sizeof (username);

    // retrieve the username.
    if (!::GetUserName(username, &length))
      throw std::runtime_error("unable to retrieve the username");

    // assign the username.
    Infinit::User.assign(username, length);
    // XXX[this will take the current user and therefore displays:
    //     `enter your passphrase ...`]
  }
#else
# error "unsupported platform"
#endif

  // retrieve the network name.
  if (Infinit::Parser->Value("Network",
                             Infinit::Network) == elle::StatusError)
    {
      // display the usage.
      Infinit::Parser->Usage();

      throw std::runtime_error("unable to retrieve the network name");
    }

  // retrieve the mount point.
  if (Infinit::Parser->Value("Mountpoint",
                             Infinit::Mountpoint) == elle::StatusError)
    {
      // display the usage.
      Infinit::Parser->Usage();

      throw std::runtime_error("unable to retrieve the mount point");
    }

  // initialize the nucleus library.
  if (nucleus::Nucleus::Initialize() == elle::StatusError)
    throw std::runtime_error("unable to initialize Nucleus");

  // initialize the Lune library.
  if (lune::Lune::Initialize() == elle::StatusError)
    throw std::runtime_error("unable to initialize Lune");

  // initialize Infinit.
  if (Infinit::Initialize() == elle::StatusError)
    throw std::runtime_error("unable to initialize Infinit");

  hole::Hole::Initialize();

  // initialize the Agent library.
  if (agent::Agent::Initialize() == elle::StatusError)
    throw std::runtime_error("unable to initialize Agent");

  // initialize the Etoile library.
  if (etoile::Etoile::Initialize() == elle::StatusError)
    throw std::runtime_error("unable to initialize Etoile");

  // initialize the horizon.
  if (horizon::Horizon::Initialize() == elle::StatusError)
    throw std::runtime_error("unable to initialize the horizon");

  // launch the program.
  elle::Program::Launch();

  // delete the parser.
  delete Infinit::Parser;
  Infinit::Parser = nullptr;

  // clean the horizon.
  if (horizon::Horizon::Clean() == elle::StatusError)
    throw std::runtime_error("unable to clean the horizon");

  // clean the Etoile library.
  if (etoile::Etoile::Clean() == elle::StatusError)
    throw std::runtime_error("unable to clean Etoile");

  // clean the Agent library.
  if (agent::Agent::Clean() == elle::StatusError)
    throw std::runtime_error("unable to clean Agent");

  // clean Hole.
  if (hole::Hole::Clean() == elle::StatusError)
    throw std::runtime_error("unable to clean Hole");

  // clean Infinit.
  if (Infinit::Clean() == elle::StatusError)
    throw std::runtime_error("unable to clean Infinit");

  // clean Lune
  if (lune::Lune::Clean() == elle::StatusError)
    throw std::runtime_error("unable to clean Lune");

  // clean the nucleus library.
  if (nucleus::Nucleus::Clean() == elle::StatusError)
    throw std::runtime_error("unable to clean Nucleus");

  // clean Elle.
  if (elle::Elle::Clean() == elle::StatusError)
    throw std::runtime_error("unable to clean Elle");
}

elle::Status
Main(elle::Natural32 argc, elle::Character* argv[])
{
  try
    {
      Infinit(argc, argv);
    }
  catch (std::runtime_error& e)
    {
      std::cerr << argv[0] << ": fatal error: " << e.what() << std::endl;
      elle::concurrency::scheduler().terminate();
      return elle::StatusError;
    }
  elle::concurrency::scheduler().terminate();
  return elle::StatusOk;
}

int
main(int argc, char* argv[])
{
  reactor::Scheduler& sched = elle::concurrency::scheduler();
  reactor::VThread<elle::Status> main(sched, "Infinit main",
                                      boost::bind(&Main, argc, argv));
  sched.run();
  return main.result() == elle::StatusOk ? 0 : 1;
}
