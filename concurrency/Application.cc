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
// updated       julien quintard   [mon mar 29 00:12:06 2010]
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
	// pass a value depending on the status.
	switch (status)
	  {
	  case StatusOk:
	    {
	      // exit.
	      Application::Core->exit(EXIT_SUCCESS);

	      break;
	    }
	  case StatusError:
	  default:
	    {
	      // exit.
	      Application::Core->exit(EXIT_FAILURE);

	      break;
	    }
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

      // process the events.
      if (Application::Core->exec() == EXIT_FAILURE)
	escape("an error occured in the application");

      leave();
    }

  }
}
