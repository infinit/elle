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
// updated       julien quintard   [wed mar 24 13:34:55 2010]
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

      // lock in writing.
      Application::Control.Lock(ModeWrite);
      {
	// check the status and display the report if necessary.
	if (status == StatusError)
	  {
	    // display the report.
	    show();

	    // exit.
	    Application::Core->exit(EXIT_FAILURE);
	  }
	else
	  {
	    // exit.
	    Application::Core->exit(EXIT_SUCCESS);
	  }
      }
      Application::Control.Unlock();

      leave();
    }

    ///
    /// this method processes events.
    ///
    Status		Application::Process()
    {
      enter();

      // check the application.
      if (Application::Core == NULL)
	escape("unable to process events since the application has not "
	       "been set up");

      while (true)
	{
	  // XXX
	  ::sleep(1);
	  // XXX

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
