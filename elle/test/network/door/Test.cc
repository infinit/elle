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
// updated       julien quintard   [wed mar 17 10:34:02 2010]
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

      // setup the application.
      if (Application::Setup(argc, argv) == StatusError)
	escape("unable to set up the application");

      // launch either the client or the server.
      if (String(argv[1]) == String("server"))
	{
	  // set up the server.
	  if (server.Setup(argv[2]) == StatusError)
	    escape("unable to set up the server");

	  // start the server.
	  if (server.Run() == StatusError)
	    escape("unable to run the server");
	}
      else if (String(argv[1]) == String("client"))
	{
	  // set up the client.
	  if (client.Setup(argv[2]) == StatusError)
	    escape("unable to set up the client");

	  // start the client.
	  if (client.Run() == StatusError)
	    escape("unable to run the client");
	}
      else
	escape("unknown type");

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
