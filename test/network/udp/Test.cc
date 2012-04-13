//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [wed jan 28 11:22:24 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/test/network/udp/Test.hh>

namespace elle
{
  namespace test
  {

//
// ---------- functions -------------------------------------------------------
//

    Status              Main(const Natural32                    argc,
                             const Character*                   argv[])
    {
      Node              node;

      // check the arguments.
      if (argc != 4)
        {
          std::cerr << "[usage] " << argv[0] << " _name_ _host_ _port_"
                    << std::endl;

          exit(1);
        }

      // init the library.
      if (Elle::Initialize() == Status::Error)
        escape("unable to initialize the Elle library");

      // set up the program
      if (Program::Setup() == Status::Error)
        escape("unable to set up the program");

      // set up the node
      if (node.Setup(String(argv[1]),
                     String(argv[2]),
                     static_cast<Port>(::strtoul(argv[3],
                                                 NULL,
                                                 0))) == Status::Error)
        escape("unable to start the node");

      // start the node.
      if (node.Run() == Status::Error)
        escape("unable to start the node");

      // launch the program.
      if (Program::Launch() == Status::Error)
        escape("an error occured during the event processing");

      // clean the library.
      if (Elle::Clean() == Status::Error)
        escape("unable to clean the Elle library");

      return Status::Ok;
    }

  }
}

//
// ---------- main ------------------------------------------------------------
//

int                     main(const int                          argc,
                             const char*                        argv[])
{
  if (elle::test::Main(argc, argv) == elle::Status::Error)
    {
      show();

      return (1);
    }

  std::cout << "[success]" << std::endl;

  return (0);
}
