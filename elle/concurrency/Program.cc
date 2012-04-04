//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [mon mar 15 20:40:02 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/system/Platform.hh>

#include <elle/concurrency/Callback.hh>
#include <elle/concurrency/Program.hh>
#include <elle/concurrency/Scheduler.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

#include <elle/idiom/Close.hh>
# include <reactor/scheduler.hh>
# include <reactor/thread.hh>
#include <elle/idiom/Open.hh>

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
    Program*                    program = NULL;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the program.
    ///
    Status              Program::Initialize()
    {
      // allocate a new program.
      program = new Program;

      return StatusOk;
    }

    ///
    /// this method cleans the program.
    ///
    Status              Program::Clean()
    {
      // delete the program.
      if (program != NULL)
        delete program;

      return StatusOk;
    }

    ///
    /// this method sets up the program for startup.
    ///
    Status              Program::Setup()
    {
      int               n;

      // set the number of arguments as zero since we do not want
      // QCoreApplication to bother parsing arguments.
      n = 0;

      // allocate the QT program.
      program->core = new ::QCoreApplication(n, NULL);

#if defined(INFINIT_LINUX) || defined(INFINIT_MACOSX)
      // set the signal handlers.
      ::signal(SIGINT, &Program::Exception);
      ::signal(SIGQUIT, &Program::Exception);
      ::signal(SIGABRT, &Program::Exception);
      ::signal(SIGTERM, &Program::Exception);
#elif defined(INFINIT_WINDOWS)
      // XXX to implement
#else
# error "unsupported platform"
#endif

      return StatusOk;
    }

    ///
    /// this method stops the program.
    ///
    Status              Program::Exit()
    {
      // exit the core application.
      program->core->exit();
      _exit = true;

      return StatusOk;
    }

    void qt_runner()
    {
      // static const reactor::Duration delay
      //   = boost::posix_time::milliseconds(10);
      while (!Program::_exit)
      {
        QCoreApplication::processEvents();
#undef yield
        scheduler().current()->yield();//sleep(delay);
      }
    }

    ///
    /// this method processes events.
    ///
    Status              Program::Launch()
    {
      // check the program.
      if ((program == NULL) || (program->core == NULL))
        escape("unable to process events since the program has not "
               "been set up");

      reactor::Thread qt(scheduler(), "Qt event loop", &qt_runner);
      scheduler().run();

      return StatusOk;
    }

    ///
    /// this method is triggered whenever a POSIX signal is received.
    ///
    Void                Program::Exception(int                  signal)
    {
      // stop the program depending on the signal.
      switch (signal)
        {
#if defined(INFINIT_LINUX) || defined(INFINIT_MACOSX)
        case SIGQUIT:
#elif defined(INFINIT_WINDOWS)
	// nothing
#else
# error "unsupported platform"
#endif
        case SIGINT:
        case SIGABRT:
        case SIGTERM:
          {
            // exit properly by finishing processing the last events.
            Program::Exit();

            break;
          }
        }
    }

#if defined(INFINIT_LINUX) || defined(INFINIT_MACOSX)
    ///
    /// this method attaches a broker to the program's event loop.
    ///
    Status              Program::Attach(Broker&                 broker)
    {
      ::QAbstractEventDispatcher*       dispatcher;

      // retrieve the event dispatcher instance.
      dispatcher = ::QAbstractEventDispatcher::instance();

      // register the socket notifier.
      dispatcher->registerSocketNotifier(&broker.notifier);

      return StatusOk;
    }

    ///
    /// this method detaches a broker from the program's event loop.
    ///
    Status              Program::Detach(Broker&                 broker)
    {
      ::QAbstractEventDispatcher*       dispatcher;

      // retrieve the event dispatcher instance.
      dispatcher = ::QAbstractEventDispatcher::instance();

      // unregister the socket notifier.
      dispatcher->unregisterSocketNotifier(&broker.notifier);

      return StatusOk;
    }
#elif defined(INFINIT_WINDOWS)
    // nothing
#else
# error "unsupported platform"
#endif

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Program::Program()
      : core(NULL)
    {}

    ///
    /// destructor.
    ///
    Program::~Program()
    {
      // releae the core.
      if (this->core != NULL)
        delete this->core;
    }

    bool Program::_exit = false;
  }
}
