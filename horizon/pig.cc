//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/pig/pig.cc
//
// created       julien quintard   [wed jun  1 09:08:15 2011]
// updated       julien quintard   [wed jun  1 09:09:50 2011]
//

#include <pig/PIG.hh>

namespace pig
{

//
// ---------- functions -------------------------------------------------------
//

  ///
  /// this is the entry point of pig.
  ///
  elle::Status		Main(elle::Natural32			argc,
			     elle::Character*			argv[])
  {
    elle::Parser*	parser;
    elle::Character	option;
    elle::String	user;
    elle::String	network;
    elle::String	mountpoint;

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

    if (parser->Register('u',
			 "user",
			 "specifies the name of the user",
			 elle::Parser::TypeRequired) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('n',
			 "network",
			 "specifies the name of the network",
			 elle::Parser::TypeRequired) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('m',
			 "mountpoint",
			 "specifies the mount point",
			 elle::Parser::TypeRequired) == elle::StatusError)
      escape("unable to register the option");

    // parse.
    while (parser->Parse(option) == elle::StatusTrue)
      {
	switch (option)
	  {
	  case 'h':
	    {
	      // display the usage.
	      parser->Usage();

	      // quit.
	      leave();
	    }
	  case 'u':
	    {
	      // retrieve the user name.
	      user.assign(optarg);

	      break;
	    }
	  case 'n':
	    {
	      // retrieve the network name.
	      network.assign(optarg);

	      break;
	    }
	  case 'm':
	    {
	      // retrieve the mountpoint.
	      mountpoint.assign(optarg);

	      break;
	    }
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

    // initialize the nucleus library.
    if (nucleus::Nucleus::Initialize() == elle::StatusError)
      escape("unable to initialize Nucleus");

    // initialize the Lune library.
    if (lune::Lune::Initialize() == elle::StatusError)
      escape("unable to initialize Lune");

    // check the user.
    if (user.empty() == true)
      {
	// display the usage.
	parser->Usage();

	escape("please specify a user name");
      }

    // check the network.
    if (network.empty() == true)
      {
	// display the usage.
	parser->Usage();

	escape("please specify a network name");
      }

    // check the mountpoint.
    if (mountpoint.empty() == true)
      {
	// display the usage.
	parser->Usage();

	escape("please specify a mountpoint");
      }

    // XXX etoile init

    // initialize PIG.
    if (PIG::Initialize(user, network, mountpoint) == elle::StatusError)
      escape("unable to initialize PIG");

    // launch the program.
    if (elle::Program::Launch() == elle::StatusError)
      escape("an error occured while processing events");

    // clean PIG.
    if (PIG::Clean() == elle::StatusError)
      escape("unable to clean PIG");

    // delete the parser.
    delete parser;

    // XXX etoile clean

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

int			main(int				argc,
			     char*				argv[])
{
  pig::Main(argc, argv);

  expose();

  return (0);
}
