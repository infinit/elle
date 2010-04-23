//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/test/fiber/Test.cc
//
// created       julien quintard   [wed jan 28 11:22:24 2009]
// updated       julien quintard   [fri apr  9 16:05:13 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include "Test.hh"

namespace elle
{
  namespace test
  {

/* XXX

    F1 -> wait ResourceA -> awaken ResourceB
    F2 -> wait ResourceB -> awaken ResourceC
    F3 -> spawn F4 -> awaken ResourceA
       -> wait ResourceC

    F1, F2, F3, F4, F3, F1, F2, F3

*/

//
// ---------- definitions -----------------------------------------------------
//

    Resource		Test::ResourceA;
    Resource		Test::ResourceB;
    Resource		Test::ResourceC;

    Timer		Test::Timer1;
    Timer		Test::Timer2;
    Timer		Test::Timer3;

//
// ---------- functions -------------------------------------------------------
//

    ///
    /// this function, invoked through in a fiber by Fiber3, simply awakens
    /// fibers waiting for ResourceA.
    ///
    Status		Fiber4()
    {
      enter();

      printf("[Fiber4] Start\n");

      printf("[Fiber4] Awaken(ResourceA)\n");

      // awaken ResourceA.
      if (Fiber::Awaken(&Test::ResourceA) == StatusError)
	escape("unable to awaken the fiber");

      printf("[Fiber4] /Awaken(ResourceA)\n");

      printf("[Fiber4] End\n");

      leave();
    }

    ///
    /// finally, this function, also launched in a fiber, performs the
    /// following steps: spawn a new fiber, launching Fiber4, and
    /// and wait for the ResourceC.
    ///
    Status		Fiber3()
    {
      Entrance<>	fiber4(&Fiber4);
      Closure<>		closure(fiber4);

      enter();

      printf("[Fiber3] Start\n");

      printf("[Fiber3] Spawn(Fiber4)\n");

      // spawn a new fiber.
      if (Fiber::Spawn(closure) == StatusError)
	escape("unable to spawn the fiber");

      printf("[Fiber3] /Spawn(Fiber4)\n");

      printf("[Fiber3] Wait(ResourceC)\n");

      // wait ResourceC.
      if (Fiber::Wait(&Test::ResourceC) == StatusError)
	escape("unable to wait for the resource");

      printf("[Fiber3] /Wait(ResourceC)\n");

      printf("[Fiber3] End\n");

      leave();
    }

    ///
    /// this function is launched in a fiber and performs the following steps:
    /// wait for the ResourceB, then awaken fibers waiting for the
    /// ResourceC.
    ///
    Status		Fiber2()
    {
      enter();

      printf("[Fiber2] Start\n");

      printf("[Fiber2] Wait(ResourceB)\n");

      // wait for ResourceB.
      if (Fiber::Wait(&Test::ResourceB) == StatusError)
	escape("unable to wait for the resource");

      printf("[Fiber2] /Wait(ResourceB)\n");

      printf("[Fiber2] Awaken(ResourceC)\n");

      // awaken ResourceC.
      if (Fiber::Awaken(&Test::ResourceC) == StatusError)
	escape("unable to awaken the resource");

      printf("[Fiber2] /Awaken(ResourceC)\n");

      printf("[Fiber2] End\n");

      leave();
    }

    ///
    /// this function is launched in a fiber and performs the following steps:
    /// wait for the ResourceA, then awaken fibers waiting for the
    /// ResourceB.
    ///
    Status		Fiber1()
    {
      enter();

      printf("[Fiber1] Start\n");

      printf("[Fiber1] Wait(ResourceA)\n");

      // wait for ResourceA.
      if (Fiber::Wait(&Test::ResourceA) == StatusError)
	escape("unable to wait for the resource");

      printf("[Fiber1] /Wait(ResourceA)\n");

      printf("[Fiber1] Awaken(ResourceB)\n");

      // awaken ResourceB.
      if (Fiber::Awaken(&Test::ResourceB) == StatusError)
	escape("unable to awaken the resource");

      printf("[Fiber1] /Awaken(ResourceB)\n");

      printf("[Fiber1] End\n");

      leave();
    }

    ///
    /// the main function.
    ///
    Status		Main(const Natural32			argc,
			     const Character*			argv[])
    {
      Callback<>	fiber1(&Fiber1);
      Callback<>	fiber2(&Fiber2);
      Callback<>	fiber3(&Fiber3);

      enter();

      // initialize the library.
      if (Elle::Initialize() == StatusError)
	escape("unable to initialize the library");

      // setup the program.
      if (Program::Setup(argc, argv) == StatusError)
	escape("unable to set up the program");

      // create and start the timer1, launching the fiber1.
      if (Test::Timer1.Create(Timer::ModeSingle, fiber1) == StatusError)
	escape("unable to create the timer");

      Test::Timer1.Start(100);

      // create and start the timer2, launching the fiber2.
      if (Test::Timer2.Create(Timer::ModeSingle, fiber2) == StatusError)
	escape("unable to create the timer");

      Test::Timer2.Start(1000);

      // create and start the timer3, launching the fiber3.
      if (Test::Timer3.Create(Timer::ModeSingle, fiber3) == StatusError)
	escape("unable to create the timer");

      Test::Timer3.Start(10000);

      // launch the program.
      if (Program::Launch() == StatusError)
	escape("an error occured in the program");

      // clean the library.
      if (Elle::Clean() == StatusError)
	escape("unable to clean the library");

      leave();
    }

  }
}

//
// ---------- main ------------------------------------------------------------
//

int			main(const int				argc,
			     const char*			argv[])
{
  if (elle::test::Main(argc, argv) == elle::miscellaneous::StatusError)
    {
      show();

      return (1);
    }

  std::cout << "[success]" << std::endl;

  return (0);
}
