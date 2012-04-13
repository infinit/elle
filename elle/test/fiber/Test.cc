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

#include "Test.hh"

namespace elle
{
  namespace test
  {

//
// ---------- definitions -----------------------------------------------------
//

    Resource            Test::ResourceA;
    Resource            Test::ResourceB;
    Resource            Test::ResourceC;

    Timer               Test::Timer1;
    Timer               Test::Timer2;
    Timer               Test::Timer3;

//
// ---------- functions -------------------------------------------------------
//

    ///
    /// this function, invoked through in a fiber by Fiber3, simply awakens
    /// fibers waiting for ResourceA.
    ///
    Status              Fiber4()
    {
      printf("[Fiber4] Start\n");

      printf("[Fiber4] Sleep()\n");

      // sleep.
      if (Fiber::Sleep(3000) == Status::Error)
        escape("unable to sleep");

      printf("[Fiber4] /Sleep()\n");

      printf("[Fiber4] Awaken(ResourceA)\n");

      // awaken ResourceA.
      if (Fiber::Awaken(Test::ResourceA) == Status::Error)
        escape("unable to awaken the fiber");

      printf("[Fiber4] /Awaken(ResourceA)\n");

      printf("[Fiber4] End\n");

      return Status::Ok;
    }

    ///
    /// finally, this function, also launched in a fiber, performs the
    /// following steps: spawn a new fiber, launching Fiber4, and
    /// and wait for the ResourceC.
    ///
    Status              Fiber3()
    {
      Closure< Status::,
               Parameters<> >   closure(Callback<>::Infer(&Fiber4));

      printf("[Fiber3] Start\n");

      printf("[Fiber3] Spawn(Fiber4)\n");

      // spawn a new fiber.
      if (Fiber::Spawn(closure) == Status::Error)
        escape("unable to spawn the fiber");

      printf("[Fiber3] /Spawn(Fiber4)\n");

      printf("[Fiber3] Wait(ResourceC)\n");

      // wait ResourceC.
      if (Fiber::Wait(Test::ResourceC) == Status::Error)
        escape("unable to wait for the resource");

      printf("[Fiber3] /Wait(ResourceC)\n");

      printf("[Fiber3] End\n");

      // exit.
      if (Program::Exit() == Status::Error)
        escape("unable to exit from the program");

      return Status::Ok;
    }

    ///
    /// this function is launched in a fiber and performs the following steps:
    /// wait for the ResourceB, then awaken fibers waiting for the
    /// ResourceC.
    ///
    Status              Fiber2()
    {
      printf("[Fiber2] Start\n");

      printf("[Fiber2] Wait(ResourceB)\n");

      // wait for ResourceB.
      if (Fiber::Wait(Test::ResourceB) == Status::Error)
        escape("unable to wait for the resource");

      printf("[Fiber2] /Wait(ResourceB)\n");

      printf("[Fiber2] Awaken(ResourceC)\n");

      // awaken ResourceC.
      if (Fiber::Awaken(Test::ResourceC) == Status::Error)
        escape("unable to awaken the resource");

      printf("[Fiber2] /Awaken(ResourceC)\n");

      printf("[Fiber2] End\n");

      return Status::Ok;
    }

    ///
    /// this function is launched in a fiber and performs the following steps:
    /// wait for the ResourceA, then awaken fibers waiting for the
    /// ResourceB.
    ///
    Status              Fiber1()
    {
      printf("[Fiber1] Start\n");

      printf("[Fiber1] Wait(ResourceA)\n");

      // wait for ResourceA.
      if (Fiber::Wait(Test::ResourceA) == Status::Error)
        escape("unable to wait for the resource");

      printf("[Fiber1] /Wait(ResourceA)\n");

      printf("[Fiber1] Awaken(ResourceB)\n");

      // awaken ResourceB.
      if (Fiber::Awaken(Test::ResourceB) == Status::Error)
        escape("unable to awaken the resource");

      printf("[Fiber1] /Awaken(ResourceB)\n");

      printf("[Fiber1] End\n");

      return Status::Ok;
    }

    ///
    /// the test's entry point.
    ///
    Status              Main(const Natural32,
                             const Character*[])
    {
      // initialize the library.
      if (Elle::Initialize() == Status::Error)
        escape("unable to initialize the library");

      // setup the program.
      if (Program::Setup() == Status::Error)
        escape("unable to set up the program");

      // create the timer1.
      if (Test::Timer1.Create(Timer::ModeSingle) == Status::Error)
        escape("unable to create the timer");

      // subscribe to the timer's signal.
      if (Test::Timer1.signal.timeout.Subscribe(
            Callback<>::Infer(&Fiber1)) == Status::Error)
        escape("unable to subscribe to the signal");

      // start the timer1, launching the fiber1.
      if (Test::Timer1.Start(100) == Status::Error)
        escape("unable to start the timer");

      // create the timer2.
      if (Test::Timer2.Create(Timer::ModeSingle) == Status::Error)
        escape("unable to create the timer");

      // subscribe to the timer's signal.
      if (Test::Timer2.signal.timeout.Subscribe(
            Callback<>::Infer(&Fiber2)) == Status::Error)
        escape("unable to subscribe to the signal");

      // start the timer2, launching the fiber2.
      if (Test::Timer2.Start(1000) == Status::Error)
        escape("unable to start the timer");

      // create the timer3.
      if (Test::Timer3.Create(Timer::ModeSingle) == Status::Error)
        escape("unable to create the timer");

      // subscribe to the timer's signal.
      if (Test::Timer3.signal.timeout.Subscribe(
            Callback<>::Infer(&Fiber3)) == Status::Error)
        escape("unable to subscribe to the signal");

      // start the timer3, launching the fiber3.
      if (Test::Timer3.Start(10000) == Status::Error)
        escape("unable to start the timer");

      // launch the program.
      if (Program::Launch() == Status::Error)
        escape("an error occured in the program");

      // clean the library.
      if (Elle::Clean() == Status::Error)
        escape("unable to clean the library");

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
