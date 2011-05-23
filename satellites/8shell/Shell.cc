//
// ---------- header ----------------------------------------------------------
//
// project       8debug
//
// license       infinit
//
// file          /home/mycure/infinit/applications/8shell/Shell.cc
//
// created       julien quintard   [thu mar  4 17:51:46 2010]
// updated       julien quintard   [mon may 23 13:36:37 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <applications/8shell/Shell.hh>

namespace application
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const elle::Character		Component[] = "8shell";

  ///
  /// this variable contains the address of the current object.
  ///
  nucleus::Address		Shell::Address;

  ///
  /// the shell commands.
  ///
  Shell::Command		Commands[] =
    {
      /*
      { "enter", Shell::Enter },
      { "list", Shell::List },
      { "select", Shell::Select },
      { "location", Shell::Location },
      */
      { "help", Shell::Help },
      { "quit", Shell::Quit },
      { NULL, NULL }
    };

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// XXX
  ///
  elle::Status		Shell::Help(const elle::String&		command)
  {
    elle::Natural32	i;

    enter();

    std::cout << "Commands:" << std::endl;

    for (i = 0; Commands[i].name; i++)
      std::cout << "  " << Commands[i].name << std::endl;

    leave();
  }

  ///
  /// XXX
  ///
  elle::Status		Shell::Quit(const elle::String&		command)
  {
    enter();

    // exit.
    ::exit(0);

    leave();
  }

//
// ---------- functions -------------------------------------------------------
//

  ///
  /// the main function.
  ///
  elle::Status		Main(elle::Natural32			argc,
			     elle::Character*			argv[])
  {
    elle::Parser*	parser;
    elle::Character	option;
    elle::Character*	line;

    enter();

    // initialize the Elle library.
    if (elle::Elle::Initialize() == elle::StatusError)
      escape("unable to initialize Elle");

    // set up the program.
    if (elle::Program::Setup() == elle::StatusError)
      escape("unable to set up the program");

    // allocate a new parser.
    parser = new elle::Parser(argc, argv);

    // set up the parser.
    if (parser->Register('h',
			 "help",
			 "display the help",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('n',
			 "network",
			 "specifies the name of the network",
			 elle::Parser::TypeRequired) == elle::StatusError)
      escape("unable to register the option");

    // parse.
    while (parser->Parse(option) == elle::StatusTrue)
      {
	// act according to the current option.
	switch (option)
	  {
	  case '?':
	    {
	      // display the usage.
	      parser->Usage();

	      escape("unknown option");
	    }
	  case ':':
	    {
	      // display the usage.
	      parser->Usage();

	      escape("missing argument");
	    }
	  default:
	    {
	      escape("an error occured while parsing the options");
	    }
	  }
      }

    // check the arguments.
    // XXX
    //if (network == nucleus::Network::Null)
    //escape("a network must be provided");

    // load the network.
    {
      // does the descriptor exist.
      // XXX

      // load the descriptor.
      // XXX

      // set the address.
      // XXX
    }

    // initialize the nucleus library.
    if (nucleus::Nucleus::Initialize() == elle::StatusError)
      escape("unable to initialize Nucleus");

    // initialize the Lune library.
    if (lune::Lune::Initialize() == elle::StatusError)
      escape("unable to initialize Lune");

    // wait for and trigger commands.
    while ((line = ::readline("$> ")) != NULL)
      {
	elle::Natural32	i;

	// look for the command.
	for (i = 0; Commands[i].name != NULL; i++)
	  {
	    // trigger the command.
	    if (elle::String(Commands[i].name) == elle::String(line))
	      {
		if (Commands[i].function(line) == elle::StatusError)
		  show();

		break;
	      }
	  }

	// no command found.
	if (Commands[i].name == NULL)
	  std::cerr << "Unknown command '" << line << "'" << std::endl;
      }

    // delete the parser.
    delete parser;

    // clean Lune
    if (lune::Lune::Clean() == elle::StatusError)
      escape("unable to clean Lune");

    // clean the nucleus library.
    if (nucleus::Nucleus::Clean() == elle::StatusError)
      escape("unable to clean Nucleus");

    // clean Elle.
    if (elle::Elle::Clean() == elle::StatusError)
      escape("unable to clean Elle");

    leave();
  }

}

//
// ---------- main ------------------------------------------------------------
//

///
/// this is the program entry point.
///
int			main(int				argc,
                             char**				argv)
{
  application::Main(argc, argv);

  expose();

  return (0);
}
