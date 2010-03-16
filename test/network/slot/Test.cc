//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/test/network/slot/Test.cc
//
// created       julien quintard   [wed jan 28 11:22:24 2009]
// updated       julien quintard   [tue mar 16 00:05:55 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/test/network/slot/Test.hh>

namespace elle
{
  namespace test
  {

//
// ---------- methods ---------------------------------------------------------
//

    Status		Test::Main(int				argc,
				   char*			argv[])
    {
      Node		node;

      enter();

      // check the arguments.
      if (argc != 4)
	{
	  std::cerr << "[usage] " << argv[0] << " _name_ _host_ _port_"
		    << std::endl;

	  exit(1);
	}

      // init the library.
      if (Elle::Initialize() == StatusError)
	escape("unable to initialize the Elle library");

      // set up the application
      if (Application::Setup(argc, argv) == StatusError)
	escape("unable to set up the application");

      // set up the node
      if (node.Setup(String(argv[1]),
		     String(argv[2]),
		     (Port)::strtoul(argv[3], NULL, NULL)) == StatusError)
	escape("unable to start the node");

      // start the node.
      if (node.Run() == StatusError)
	escape("unable to start the node");

      // process the events.
      if (Application::Process() == StatusError)
	escape("an error occured during the event processing");

      // clean the library.
      if (Elle::Clean() == StatusError)
	escape("unable to clean the Elle library");

      leave();
    }

  }
}

//
// ---------- main ------------------------------------------------------------
//

int			main(int				argc,
			     char**				argv)
{
  if (elle::test::Test::Main(argc, argv) == elle::misc::StatusError)
    {
      std::cerr << elle::misc::report << std::endl;

      return (1);
    }

  std::cout << "[success]" << std::endl;

  return (0);
}
