//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/hole.cc
//
// created       julien quintard   [wed may 11 15:20:51 2011]
// updated       julien quintard   [wed may 25 10:16:06 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/Hole.hh>

#include <lune/Lune.hh>

#include <elle/idiom/Close.hh>
# include <list>
#include <elle/idiom/Open.hh>

namespace hole
{

//
// ---------- functions -------------------------------------------------------
//

  ///
  /// the main function.
  ///
  elle::Status		Main(elle::Natural32			argc,
			     elle::Character*			argv[])
  {
    elle::Parser*		parser;
    std::list<elle::String>	list;
    elle::Character		option;

    enter();

    // initialize the Elle library.
    if (elle::Elle::Initialize() == elle::StatusError)
      escape("unable to initialize the Elle library");

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
			 "join the given network at startup",
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
	  case 'n':
	    {
	      // add the name to the list.
	      list.push_back(optarg);

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

    // initialize the lune library.
    if (lune::Lune::Initialize() == elle::StatusError)
      escape("unable to initialize the Lune library");

    // initialize the hole.
    if (Hole::Initialize() == elle::StatusError)
      escape("unable to initialize the hole");

    // go through the list and join the networks.
    while (list.empty() == false)
      {
	elle::String		name;
	nucleus::Network	network;

	// retrieve the name.
	name = list.front();

	// create a network object.
	if (network.Create(name) == elle::StatusError)
	  escape("unable to create the network");

	// check if this network has already been joined. if not, join it.
	if (Hole::Networks.find(network) == Hole::Networks.end())
	  {
	    Holeable*					holeable;
	    nucleus::Address				root;
	    elle::Address				boot;
	    std::pair<Hole::Iterator, elle::Boolean>	result;
	    Model					model;

	    // XXX[to change: contact the meta disciple]
	    {
	      lune::Descriptor	descriptor;

	      if (descriptor.Load(network.name) == elle::StatusError)
		escape("unable to load the descriptor");

	      model = descriptor.model;
	      root = descriptor.root;
	      boot = descriptor.boot;
	    }
	    // XXX

	    // create the holeable depending on the implementation.
	    switch (model)
	      {
	      case ModelLocal:
		{
		  // allocate the instance.
		  holeable = new local::Local(network);

		  break;
		}
	      case ModelRemote:
		{
		  remote::Remote*	remote;

		  // allocate the instance.
		  remote = new remote::Remote(network);

		  // set remote host.
		  if (remote->Host(boot) == elle::StatusError)
		    escape("unable to set the host");

		  // set the holeable.
		  holeable = remote;

		  break;
		}
	      default:
		escape("unknown model");
	      }

	    // set the root address.
	    if (holeable->Root(root) == elle::StatusError)
	      escape("unable to set the root address");

	    // insert the network in the container.
	    result = Hole::Networks.insert(
		       std::pair<nucleus::Network,
				 Holeable*>(network,
					    holeable));

	    // check if the insertion was successful.
	    if (result.second == false)
	      escape("unable to insert the network in the container");
	  }

	// pop the element.
	list.pop_front();
      }

    // launch the program.
    if (elle::Program::Launch() == elle::StatusError)
      escape("unable to process the events");

    // clean the hole.
    if (Hole::Clean() == elle::StatusError)
      escape("unable to clean the hole");

    // clean the Lune library.
    if (lune::Lune::Clean() == elle::StatusError)
      escape("unable to clean the Lune library");

    // clean the nucleus library.
    if (nucleus::Nucleus::Clean() == elle::StatusError)
      escape("unable to clean Nucleus");

    // clean the Elle library.
    if (elle::Elle::Clean() == elle::StatusError)
      escape("unable to clean the Elle library");

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
                             char*				argv[])
{
  hole::Main(argc, argv);

  expose();

  return (0);
}
