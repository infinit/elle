//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/test/network/door/Test.cc
//
// created       julien quintard   [wed jan 28 11:22:24 2009]
// updated       julien quintard   [sun mar  7 22:08:05 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/test/network/door/Test.hh>

namespace elle
{
  using namespace core;
  using namespace misc;

  namespace test
  {

//
// ---------- methods ---------------------------------------------------------
//

    Status		Test::Main(int				argc,
				   char*			argv[])
    {
      QCoreApplication	app(argc, argv);
      Server		server;
      Client		client;

      enter();

      // check the arguments.
      if (argc != 3)
	{
	  std::cerr << "[usage] " << argv[0] << " _type{client/server}_ _id_"
		    << std::endl;

	  exit(1);
	}

      // init the library.
      if (Elle::Initialize() == StatusError)
	escape("unable to initialize the Elle library");

      // launch either the client or the server.
      if (String(argv[1]) == String("server"))
	{
	  server.name = String(argv[2]);

	  // start the main thread.
	  server.start();
	}
      else if (String(argv[1]) == String("client"))
	{
	  client.name = String(argv[2]);

	  // start the main thread.
	  client.start();
	}
      else
	escape("unknown type");

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
