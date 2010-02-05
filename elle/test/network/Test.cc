//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/test/network/Test.cc
//
// created       julien quintard   [wed jan 28 11:22:24 2009]
// updated       julien quintard   [thu feb  4 00:27:15 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include "Test.hh"

namespace elle
{
  using namespace core;
  using namespace misc;
  using namespace network;

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

      // check the arguments.
      if (argc != 4)
	{
	  std::cerr << "[usage] " << argv[0] << " _name_ _host_ _port_"
		    << std::endl;

	  exit(1);
	}

      // start the node
      if (node.Start(String(argv[1]),
		     String(argv[2]),
		     (Port)::strtoul(argv[3], NULL, NULL)) == StatusError)
	escape("unable to start the node");

      // wait for events.
      app.exec();

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
