//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Program.cc
//
// created       julien quintard   [mon mar 15 20:40:02 2010]
// updated       julien quintard   [wed jul  6 13:07:52 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/concurrency/Program.hh>
#include <elle/concurrency/Callback.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

namespace elle
{
  namespace concurrency
  {

//
// ---------- globals ---------------------------------------------------------
//

    ///
    /// this variable represents the program.
    ///
    Program*			program = NULL;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the program.
    ///
    Status		Program::Initialize()
    {
      enter();

      // allocate a new program.
      program = new Program;

      leave();
    }

    ///
    /// this method cleans the program.
    ///
    Status		Program::Clean()
    {
      enter();

      // delete the program.
      if (program != NULL)
	delete program;

      leave();
    }

    ///
    /// this method sets up the program for startup.
    ///
    Status		Program::Setup(Callback< Parameters<> >* prolog,
				       Callback< Parameters<> >* epilog)
    {
      int		n;

      enter();

      // set the number of arguments as zero since we do not want
      // QCoreApplication to bother parsing arguments.
      n = 0;

      // allocate the QT program.
      program->core = new ::QCoreApplication(n, NULL);

      // set the prolog/epilog callbacks.
      program->prolog = prolog;
      program->epilog = epilog;

      // set the signal handlers.
      ::signal(SIGINT, &Program::Signal);
      ::signal(SIGQUIT, &Program::Signal);
      ::signal(SIGABRT, &Program::Signal);
      ::signal(SIGTERM, &Program::Signal);

      ::signal(SIGKILL, &Program::Signal);

      leave();
    }

    ///
    /// this method stops the program.
    ///
    Status		Program::Exit()
    {
      enter();

      // set the exit state.
      program->state = Program::StateStopped;

      leave();
    }

    ///
    /// this method processes events.
    ///
    Status		Program::Launch()
    {
      enter();

      // check the program.
      if ((program == NULL) || (program->core == NULL))
	escape("unable to process events since the program has not "
	       "been set up");

      // process the events.
      while (true)
	{
	  // check if the program must be stopped.
	  if (program->state == Program::StateStopped)
	    break;

	  // call the prolog, if provided.
	  if (program->prolog != NULL)
	    {
	      if (program->prolog->Trigger() == StatusError)
		escape("an error occured in the prolog");
	    }

	  // process the QT events.
	  program->core->processEvents();

	  // call the epilog, if provided.
	  if (program->epilog != NULL)
	    {
	      if (program->epilog->Trigger() == StatusError)
		escape("an error occured in the epilog");
	    }

	  // if there are no events left to process, sleep a bit in order
	  // to prevent using 100% of the CPU.
	  if (program->core->hasPendingEvents() == false)
	    {
	      // sleep.
	      /// XXX \todo peut-on eviter de dormir? normalement c'est
	      /// a ca que sert un select() justement.
	      ::usleep(10000);
	    }
	}

      leave();
    }

    ///
    /// this method is triggered whenever a POSIX signal is received.
    ///
    Void		Program::Signal(int			signal)
    {
      // stop the program depending on the signal.
      switch (signal)
	{
	case SIGINT:
	case SIGQUIT:
	case SIGABRT:
	case SIGTERM:
	  {
	    // exit properly by finishing processing the last events.
	    Program::Exit();

	    break;
	  }
	case SIGKILL:
	  {
	    // exit brutally!!!
	    ::exit(EXIT_FAILURE);
	  }
	}
    }

    ///
    /// this method attaches a broker to the program's event loop.
    ///
    Status		Program::Attach(Broker&			broker)
    {
      ::QAbstractEventDispatcher*	dispatcher;

      enter();

#include <elle/idiom/Close.hh>

      // retrieve the event dispatcher instance.
      dispatcher = ::QAbstractEventDispatcher::instance();

#include <elle/idiom/Open.hh>

      // register the socket notifier.
      dispatcher->registerSocketNotifier(&broker.notifier);

      leave();
    }

    ///
    /// this method detaches a broker from the program's event loop.
    ///
    Status		Program::Detach(Broker&			broker)
    {
      ::QAbstractEventDispatcher*	dispatcher;

      enter();

#include <elle/idiom/Close.hh>

      // retrieve the event dispatcher instance.
      dispatcher = ::QAbstractEventDispatcher::instance();

#include <elle/idiom/Open.hh>

      // unregister the socket notifier.
      dispatcher->unregisterSocketNotifier(&broker.notifier);

      leave();
    }

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Program::Program():
      core(NULL),
      state(Program::StateUnknown),
      prolog(NULL),
      epilog(NULL)
    {
    }

    ///
    /// destructor.
    ///
    Program::~Program()
    {
      // releae the core.
      if (this->core != NULL)
	delete this->core;
    }

  }
}
