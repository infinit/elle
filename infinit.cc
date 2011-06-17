//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit
//
// file          /home/mycure/infinit/infinit.cc
//
// created       julien quintard   [wed jun  1 10:53:21 2011]
// updated       julien quintard   [fri jun 17 17:03:25 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <Infinit.hh>

#include <nucleus/Nucleus.hh>
#include <lune/Lune.hh>
#include <agent/Agent.hh>
#include <etoile/Etoile.hh>
#include <hole/Hole.hh>
#include <pig/PIG.hh>

//
// ---------- functions -------------------------------------------------------
//

///
/// this is the entry point of pig.
///
elle::Status		Main(elle::Natural32			argc,
			     elle::Character*			argv[])
{
  elle::Parser*		parser;
  elle::Character	option;
  elle::String		user;
  elle::String		network;
  elle::String		mountpoint;

  enter(instance(parser));

  // initialize the Elle library.
  if (elle::Elle::Initialize() == elle::StatusError)
    escape("unable to initialize Elle");

  // initialize Infinit.
  if (Infinit::Initialize() == elle::StatusError)
    escape("unable to initialize Infinit");

  // set up the program.
  if (elle::Program::Setup() == elle::StatusError)
    escape("unable to set up the program");

  // allocate a new parser.
  parser = new elle::Parser(argc, argv);

  // specify a program description.
  if (parser->Description("Copyright (c) 2008, 2009, 2010, 2011, "
			  "Julien Quintard, All rights reserved.\n"
			  "\n"
			  +
			  Infinit::Version + "\n"
			  "\n") == elle::StatusError)
    escape("unable to set the description");

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

  // initialize the nucleus library.
  if (nucleus::Nucleus::Initialize() == elle::StatusError)
    escape("unable to initialize Nucleus");

  // initialize the Lune library.
  if (lune::Lune::Initialize() == elle::StatusError)
    escape("unable to initialize Lune");

  // initialize the Agent library.
  if (agent::Agent::Initialize(user) == elle::StatusError)
    escape("unable to initialize Agent");

  // initialize the Etoile library.
  if (etoile::Etoile::Initialize() == elle::StatusError)
    escape("unable to initialize Etoile");

  // initialize the Hole library.
  if (hole::Hole::Initialize(network) == elle::StatusError)
    escape("unable to initialize Hole");

  // initialize PIG.
  if (pig::PIG::Initialize(mountpoint) == elle::StatusError)
    escape("unable to initialize PIG");

  // launch the program.
  if (elle::Program::Launch() == elle::StatusError)
    escape("an error occured while processing events");

  // clean PIG.
  if (pig::PIG::Clean() == elle::StatusError)
    escape("unable to clean PIG");

  // clean Hole.
  if (hole::Hole::Clean() == elle::StatusError)
    escape("unable to clean Hole");

  // clean the Etoile library.
  if (etoile::Etoile::Clean() == elle::StatusError)
    escape("unable to clean Etoile");

  // clean the Agent library.
  if (agent::Agent::Clean() == elle::StatusError)
    escape("unable to clean Agent");

  // clean Lune
  if (lune::Lune::Clean() == elle::StatusError)
    escape("unable to clean Lune");

  // clean the nucleus library.
  if (nucleus::Nucleus::Clean() == elle::StatusError)
    escape("unable to clean Nucleus");

  // clean Infinit.
  if (Infinit::Clean() == elle::StatusError)
    escape("unable to clean Infinit");

  // clean Elle.
  if (elle::Elle::Clean() == elle::StatusError)
    escape("unable to clean Elle");

  // delete the parser.
  delete parser;

  // waive.
  waive(parser);

  leave();
}

//
// ---------- main ------------------------------------------------------------
//

int			main(int				argc,
			     char*				argv[])
{
  Main(argc, argv);

  expose();

  return (0);
}
