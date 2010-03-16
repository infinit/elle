//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Application.cc
//
// created       julien quintard   [mon mar 15 20:40:02 2010]
// updated       julien quintard   [tue mar 16 11:36:44 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/concurrency/Application.hh>

namespace elle
{
  namespace concurrency
  {

//
// ---------- globals ---------------------------------------------------------
//

    ///
    /// this variable represents the main application.
    ///
    Application*				application = NULL;

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this variable is used for controlling the concurrent accesses.
    ///
    Accord					Application::Control;

    ///
    /// this variable holds the slots waiting for a specific event.
    ///
    Application::Container			Application::Events;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Application::Application(int				argc,
			     char**				argv):
      core(argc, argv)
    {
    }

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method sets up the application for startup.
    ///
    Status		Application::Setup(int			argc,
					   char**		argv)
    {
      enter();

      // allocate the application.
      application = new Application(argc, argv);

      leave();
    }

    ///
    /// this method processes events.
    ///
    Status		Application::Process()
    {
      enter();

      // check the application.
      if (application == NULL)
	escape("unable to process events since the application has not "
	       "been set up");

      while (true)
	{
	  // lock in reading.
	  Application::Control.Lock(ModeRead);
	  {
	    // check if we should return.
	    if ((Application::Events.empty() == false) &&
		(Application::Events.front()->second != NULL))
	      {
		Application::Control.Unlock();

		break;
	      }
	  }
	  Application::Control.Unlock();

	  // process events.
	  application->core.processEvents();

	  // lock in reading.
	  Application::Control.Lock(ModeRead);
	  {
	    // if there are no events left to process, sleep a bit in order
	    // to prevent using 100% of the CPU.
	    if (application->core.hasPendingEvents() == false)
	      {
		Application::Control.Unlock();

		::usleep(10000);
	      }
	    else
	      Application::Control.Unlock();
	  }
	}

      leave();
    }

  }
}
