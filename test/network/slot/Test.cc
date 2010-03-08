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
// updated       julien quintard   [sun mar  7 23:38:09 2010]
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
      QCoreApplication	app(argc, argv);
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

      // set up the node
      if (node.Setup(String(argv[1]),
		     String(argv[2]),
		     (Port)::strtoul(argv[3], NULL, NULL)) == StatusError)
	escape("unable to start the node");

      // start the node.
      node.start();

      // wait for events.
      app.exec();

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
