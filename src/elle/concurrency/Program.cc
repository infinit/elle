#include <elle/system/platform.hh>

#include <elle/concurrency/Program.hh>

#include <reactor/scheduler.hh>
#include <reactor/signal.hh>
#include <reactor/thread.hh>
#include <signal.h>

#include <elle/log.hh>
#include <elle/CrashReporter.hh>

ELLE_LOG_COMPONENT("elle.concurrency.Program");

namespace elle
{
  namespace concurrency
  {

    std::string Program::_name{""};

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method sets up the program for startup.
    ///
    Status              Program::Setup(std::string const& name)
    {
#if defined(INFINIT_LINUX) || defined(INFINIT_MACOSX)
      // set the signal handlers.
      ::signal(SIGINT, &Program::Exception);
      ::signal(SIGQUIT, &Program::Exception);
      ::signal(SIGABRT, &Program::Exception);
      ::signal(SIGTERM, &Program::Exception);
      ::signal(SIGSEGV, &Program::Exception);
#elif defined(INFINIT_WINDOWS)
      // XXX to implement
#else
# error "unsupported platform"
#endif

      Program::_name = name;

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

      reactor::Scheduler::scheduler()->current()->wait(_exit);
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
            // exit properly by finishing processing the last events.
            Program::Exit();

            break;
          }
        case SIGSEGV: // Probability to send succesfully data is small but try.
          {
            // XXX: Backtrace is fucked by signal.
            // Maybe this should be improved?
            elle::crash::report(Program::_name, "SIGSEGV", reactor::Backtrace::current());

            ::exit(EXIT_FAILURE);

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
