//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit
//
// author        julien quintard   [wed jun  1 10:53:21 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <Infinit.hh>

#include <elle/types.hh>
#include <nucleus/Nucleus.hh>
#include <lune/Lune.hh>
#include <agent/Agent.hh>
#include <etoile/Etoile.hh>
#include <hole/Hole.hh>
#include <horizon/Horizon.hh>

//
// ---------- functions -------------------------------------------------------
//

///
/// this is the entry point of infinit.
///
elle::Status            Main(elle::Natural32                    argc,
                             elle::Character*                   argv[])
{
  // initialize the Elle library.
  if (elle::Elle::Initialize() == elle::Status::Error)
    escape("unable to initialize Elle");

  // set up the program.
  if (elle::concurrency::Program::Setup() == elle::Status::Error)
    escape("unable to set up the program");

  // allocate a new parser.
  Infinit::Parser = new elle::Parser(argc, argv);

  // specify a program description.
  if (Infinit::Parser->Description(Infinit::Copyright) == elle::Status::Error)
    escape("unable to set the description");

  // register the options.
  if (Infinit::Parser->Register(
        "Help",
        'h',
        "help",
        "display the help",
        elle::Parser::KindNone) == elle::Status::Error)
    escape("unable to register the option");

  // register the option.
  if (Infinit::Parser->Register(
        "Network",
        'n',
        "network",
        "specifies the name of the network",
        elle::Parser::KindRequired) == elle::Status::Error)
    escape("unable to register the option");

  // register the option.
  if (Infinit::Parser->Register(
        "Mountpoint",
        'm',
        "mountpoint",
        "specifies the mount point",
        elle::Parser::KindRequired) == elle::Status::Error)
    escape("unable to register the option");

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
#if defined(INFINIT_LINUX) || defined(INFINIT_MACOSX)
  {
    struct ::passwd*    pw;

    // retrieve the current password
    if ((pw = ::getpwuid(geteuid())) == NULL)
      escape("unable to retrieve the current user's password structure");

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
      escape("unable to retrieve the username");

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
                             Infinit::Network) == elle::Status::Error)
    {
      // display the usage.
      Infinit::Parser->Usage();

      escape("unable to retrieve the network name");
    }

  // retrieve the mount point.
  if (Infinit::Parser->Value("Mountpoint",
                             Infinit::Mountpoint) == elle::Status::Error)
    {
      // display the usage.
      Infinit::Parser->Usage();

      escape("unable to retrieve the mount point");
    }

  // initialize the nucleus library.
  if (nucleus::Nucleus::Initialize() == elle::Status::Error)
    escape("unable to initialize Nucleus");

  // initialize the Lune library.
  if (lune::Lune::Initialize() == elle::Status::Error)
    escape("unable to initialize Lune");

  // initialize Infinit.
  if (Infinit::Initialize() == elle::Status::Error)
    escape("unable to initialize Infinit");

  // initialize the Hole library.
  if (hole::Hole::Initialize() == elle::Status::Error)
    escape("unable to initialize Hole");

  // initialize the Agent library.
  if (agent::Agent::Initialize() == elle::Status::Error)
    escape("unable to initialize Agent");

  // initialize the Etoile library.
  if (etoile::Etoile::Initialize() == elle::Status::Error)
    escape("unable to initialize Etoile");

  // initialize the horizon.
  if (horizon::Horizon::Initialize() == elle::Status::Error)
    escape("unable to initialize the horizon");

  // launch the program.
  if (elle::concurrency::Program::Launch() == elle::Status::Error)
    escape("an error occured while processing events");

  // delete the parser.
  delete Infinit::Parser;
  Infinit::Parser = nullptr;

  // clean the horizon.
  if (horizon::Horizon::Clean() == elle::Status::Error)
    escape("unable to clean the horizon");

  // clean the Etoile library.
  if (etoile::Etoile::Clean() == elle::Status::Error)
    escape("unable to clean Etoile");

  // clean the Agent library.
  if (agent::Agent::Clean() == elle::Status::Error)
    escape("unable to clean Agent");

  // clean Hole.
  if (hole::Hole::Clean() == elle::Status::Error)
    escape("unable to clean Hole");

  // clean Infinit.
  if (Infinit::Clean() == elle::Status::Error)
    escape("unable to clean Infinit");

  // clean Lune
  if (lune::Lune::Clean() == elle::Status::Error)
    escape("unable to clean Lune");

  // clean the nucleus library.
  if (nucleus::Nucleus::Clean() == elle::Status::Error)
    escape("unable to clean Nucleus");

  // clean Elle.
  if (elle::Elle::Clean() == elle::Status::Error)
    escape("unable to clean Elle");

  return elle::Status::Ok;
}

//
// ---------- main ------------------------------------------------------------
//

int                     main(int                                argc,
                             char*                              argv[])
{
  try
    {
      if (Main(argc, argv) == elle::Status::Error)
        {
          show();

          return (1);
        }
    }
  catch (...)
    {
      std::cout << "The program has been terminated following "
                << "a fatal error" << std::endl;

      return (1);
    }

  return (0);
}
