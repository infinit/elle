//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/Hole.cc
//
// created       julien quintard   [tue apr 13 15:27:20 2010]
// updated       julien quintard   [fri may  6 14:19:15 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/Hole.hh>

namespace hole
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this is a pointer to the instance of the hole implementation.
  ///
  Hole*			Hole::Implementation = NULL;

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method initializes the hole by allocating and initializing
  /// the implementation.
  ///
  Status		Hole::Initialize(const String&		implementation)
  {
    enter();

    //
    // allocate and initializes the implementation.
    //
    {
      // allocate the implementation.
      if (implementation == "local")
	Hole::Implementation = new Local;
      // XXX
      else
	escape("unknown implementation name");

      // initialize the implementation.
      if (Hole::Implementation->Initialize() == StatusError)
	escape("unable to initialize the implementation");
    }

    //
    // register the inward messages.
    //
    {
      Callback<const Report>			error(&Hole::Error);
      Callback<const Address, const Data>	put(&Hole::Put);
      Callback<const Address>			get(&Hole::Get);
      Callback<const Address>			destroy(&Hole::Destroy);

      // register the error message.
      if (Network::Register<TagError>(error) == StatusError)
        escape("unable to register the error callback");

      // register the put message.
      if (Network::Register<TagPut>(put) == StatusError)
        escape("unable to register the put callback");

      // register the get message.
      if (Network::Register<TagGet>(get) == StatusError)
        escape("unable to register the get callback");

      // register the destroy message.
      if (Network::Register<TagDestroy>(destroy) == StatusError)
        escape("unable to register the destroy callback");
    }

    leave();
  }

  ///
  /// this method cleans the hole.
  ///
  Status		Hole::Clean()
  {
    enter();

    // clean the implementation.
    if (Hole::Implementation->Clean() == StatusError)
      escape("unable to clean the implementation");

    // delete the implementation.
    delete Hole::Implementation;

    // reset the pointer.
    Hole::Implementationn = NULL;

    leave();
  }

//
// ---------- functions -------------------------------------------------------
//

  ///
  /// the main function.
  ///
  Status		Main(const Natural32			argc,
			     const Character*			argv[])
  {
    String		implementation;

    enter();

    /// XXX \todo to improve with real argument parsing.
    {
      if (argc != 2)
	escape("[usage] ./hole {implementation}");

      implementation.assign(argv[1]);
    }

    // initialize the Elle library.
    if (Elle::Initialize() == StatusError)
      escape("unable to initialize the Elle library");

    // XXX init/clean nucleus

    // set up the program.
    if (Program::Setup(argc, argv) == StatusError)
      escape("unable to set up the program");

    // initialize the hole.
    if (Hole::Initialize(implemetation) == StatusError)
      escape("unable to initialize the hole");

    // launch the program.
    if (Program::Launch() == StatusError)
      escape("unable to process the events");

    // clean the hole.
    if (Hole::Clean() == StatusError)
      escape("unable to clean the hole");

    // clean the Elle library.
    if (Elle::Clean() == StatusError)
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
int			main(const int				argc,
                             const char*			argv[])
{
  hole::Main(argc, argv);

  expose();

  return (0);
}
