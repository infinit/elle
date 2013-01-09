#include <elle/system/platform.hh>

#include <elle/concurrency/Program.hh>
#include <elle/concurrency/Scheduler.hh>

#include <elle/idiom/Close.hh>
# include <reactor/scheduler.hh>
# include <reactor/signal.hh>
# include <reactor/thread.hh>
# include <signal.h>
#include <elle/idiom/Open.hh>

#include <elle/log.hh>

ELLE_LOG_COMPONENT("elle.concurrency.Program");

namespace elle
{
  namespace concurrency
  {
//
// ---------- static attribules --------------------------------------------------
//
    elle::signal::ScoppedGuard Program::_signal_guard{};

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method sets up the program for startup.
    ///
    Status              Program::Setup()
    {
      Program::_signal_guard.init(
        {
#if defined(INFINIT_LINUX) || defined(INFINIT_MACOSX)
          SIGQUIT,
#elif defined(INFINIT_WINDOWS)
            // Nothing.
#else
# error "unsupported platform"
#endif
          SIGINT, SIGABRT, SIGTERM, SIGSEGV},
        [] (boost::system::error_code const&, int sig)
        {
          Program::Exception(sig);
        }
      );

      return Status::Ok;
    }

    void
    Program::Exit()
    {
      ELLE_TRACE_SCOPE("Exit");

      _exit.signal();
    }

    void
    Program::Launch()
    {
      ELLE_TRACE_SCOPE("Launch");

      concurrency::scheduler().current()->wait(_exit);
    }

    ///
    /// this method is triggered whenever a POSIX signal is received.
    ///
    Void                Program::Exception(int                  signal)
    {
      ELLE_TRACE_SCOPE("Exception");

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
            Program::_signal_guard.release();

            // exit properly by finishing processing the last events.
            Program::Exit();

            break;
          }
        }
    }

//
// ---------- signals ---------------------------------------------------------
//

    reactor::Signal Program::_exit;
  }
}
