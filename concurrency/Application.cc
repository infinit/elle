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
// updated       julien quintard   [mon mar 22 20:57:43 2010]
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
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this variable is used for controlling the concurrent accesses.
    ///
    Accord				Application::Control;

    ///
    /// this variable holds the slots waiting for a specific event.
    ///
    Application::Container		Application::Events;

    ///
    /// this variable holds the QT application.
    ///
    ::QCoreApplication*			Application::Core = NULL;

    ///
    /// this variable holds the application flags.
    ///
    Application::Flags			Application::Flag =
      Application::FlagNone;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the application.
    ///
    Status		Application::Initialize()
    {
      enter();

      // nothing to do

      leave();
    }

    ///
    /// this method cleans the application.
    ///
    Status		Application::Clean()
    {
      enter();

      /// XXX /todo remove the registered waiting events

      leave();
    }

    ///
    /// this method sets up the application for startup.
    ///
    Status		Application::Setup(int			argc,
					   char**		argv)
    {
      enter();

      // allocate the application.
      Application::Core = new ::QCoreApplication(argc, argv);

      leave();
    }

    ///
    /// this method stops the application.
    ///
    Status		Application::Exit(Status		status)
    {
      enter();

      // check the status and display the report if necessary.
      if (status != StatusOk)
	show();

      /// XXX \todo this is what should be done but since blocking frames
      /// are not supported yet, the application quit manually instead of
      /// setting the flag.
      /*
      // lock in writing.
      Application::Control.Lock(ModeWrite);
      {
	// set the exit flag.
	Application::Flag = Application::FlagExit;
      }
      Application::Control.Unlock();
      */

      // XXX
      ::exit(0);

      leave();
    }

    ///
    /// this method processes events.
    ///
    Status		Application::Process()
    {
      enter();

      printf("[XXX] Application::Process()\n");

      // check the application.
      if (Application::Core == NULL)
	escape("unable to process events since the application has not "
	       "been set up");

      while (true)
	{
	  // XXX
	  //printf("Processing...\n");
	  //::sleep(1);
	  // XXX

	  // lock in reading.
	  Application::Control.Lock(ModeRead);
	  {
	    // check if we should quit or return.
	    if ((Application::Flag == Application::FlagExit) ||
		((Application::Events.empty() == false) &&
		 (Application::Events.front()->second != NULL)))
	      {
		Application::Control.Unlock();

		break;
	      }
	  }
	  Application::Control.Unlock();

	  // process events.
	  Application::Core->processEvents();

	  // lock in reading.
	  Application::Control.Lock(ModeRead);
	  {
	    // if there are no events left to process, sleep a bit in order
	    // to prevent using 100% of the CPU.
	    if (Application::Core->hasPendingEvents() == false)
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
