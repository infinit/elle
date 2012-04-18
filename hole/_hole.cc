//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [wed may 11 15:20:51 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <Infinit.hh>
#include <elle/types.hh>
#include <nucleus/Nucleus.hh>
#include <lune/Lune.hh>
#include <hole/Hole.hh>

namespace hole
{

//
// ---------- functions -------------------------------------------------------
//

  ///
  /// the main function.
  ///
  elle::Status          Main(elle::Natural32                    argc,
                             elle::Character*                   argv[])
  {
    // XXX Infinit::Parser is not deleted in case of error

    // initialize the Elle library.
    if (elle::Elle::Initialize() == elle::Status::Error)
      escape("unable to initialize Elle");

    // initialize the nucleus library.
    if (nucleus::Nucleus::Initialize() == elle::Status::Error)
      escape("unable to initialize Nucleus");

    // initialize the Lune library.
    if (lune::Lune::Initialize() == elle::Status::Error)
      escape("unable to initialize Lune");

    // initialize Infinit.
    if (Infinit::Initialize() == elle::Status::Error)
      escape("unable to initialize Infinit");

    // set up the program.
    if (elle::concurrency::Program::Setup() == elle::Status::Error)
      escape("unable to set up the program");

    // allocate a new parser.
    Infinit::Parser = new elle::Parser(argc, argv);

    // specify a program description.
    if (Infinit::Parser->Description(Infinit::Version +
                                     " "+
                                     "Copyright (c) 2008, 2009, 2010, 2011, "
                                     "Julien Quintard, All rights "
                                     "reserved.\n") == elle::Status::Error)
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

    // retrieve the network name.
    if (Infinit::Parser->Value("Network",
                               Infinit::Network) == elle::Status::Error)
      {
        // display the usage.
        Infinit::Parser->Usage();

        escape("unable to retrieve the network name");
      }

    // initialize the Hole library.
    if (hole::Hole::Initialize() == elle::Status::Error)
      escape("unable to initialize Hole");

    // launch the program.
    if (elle::concurrency::Program::Launch() == elle::Status::Error)
      escape("an error occured while processing events");

    // delete the parser.
    delete Infinit::Parser;
    Infinit::Parser = nullptr;

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

}

//
// ---------- main ------------------------------------------------------------
//

///
/// this is the program entry point.
///
int                     main(int                                argc,
                             char*                              argv[])
{
  try
    {
      if (hole::Main(argc, argv) == elle::Status::Error)
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
