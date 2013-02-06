#include <common/common.hh>

#include <elle/format/json.hh>
#include <elle/concurrency/Scheduler.hh>
#include <elle/os/path.hh>
#include <elle/log.hh>
#include <elle/HttpClient.hh>
#include <elle/system/platform.hh>

#include "CrashReporter.hh"

#include <fstream>
#include <map>
#include <signal.h>

#include <elle/system/platform.hh>
#if defined(INFINIT_MACOSX)
extern char** environ;
#endif

ELLE_LOG_COMPONENT("elle.CrashReporter");

namespace elle
{
  namespace signal
  {
    ScopedGuard::ScopedGuard(std::vector<int> const& sig,
                               Handler const& handler):
      _signals{elle::concurrency::scheduler().io_service()},
      _handler{handler}
    {
      // Each guard manager a specific handler but any signals.
      std::for_each(sig.begin(), sig.end(),
                    [&](int sig) { ELLE_DEBUG("handling %s", sig);
                                   this->_signals.add(sig); });

      //XXX: this->_launch();
    }

    ScopedGuard::~ScopedGuard()
    {
      //XXX: this->_release();
    }

    void
    ScopedGuard::_launch()
    {
      ELLE_TRACE("launching guard");
      this->_signals.async_wait(this->_handler);
    }

    void
    ScopedGuard::_release()
    {
      ELLE_TRACE("releasing guard");
      this->_signals.cancel();
    }

    namespace
    {
      // It seems that strsignal from signal.h isn't portable on every os.
      // This map contains the main posix signals and make can easy be multi-
      // platform by using defines for specific os.
      std::string const&
      strsignal(int signal)
      {
        static std::map<int, std::string> bind(
          {
            {SIGHUP,  "SIGHUP"},  // Hangup detected on controlling terminal or death of controlling process
            {SIGINT,  "SIGINT"},  // Interrupt from keyboard
            {SIGQUIT, "SIGQUIT"}, // Quit from keyboard
            {SIGILL,  "SIGILL"},  // Illegal Instruction
            {SIGABRT, "SIGABRT"}, // Abort signal from abort(3)
            {SIGFPE,  "SIGFPE"},  // Floating point exception
            {SIGKILL, "SIGKILL"}, // Kill signal
            {SIGSEGV, "SIGSEGV"}, // Invalid memory reference
            {SIGPIPE, "SIGPIPE"}, // Broken pipe: write to pipe with no readers
            {SIGALRM, "SIGALRM"}, // Timer signal from alarm(2)
            {SIGTERM, "SIGTERM"}, // Termination signal
            {SIGCHLD, "SIGCHLD"}, // Child stopped or terminated
            {SIGCONT, "SIGCONT"}, // Continue if stopped
            {SIGSTOP, "SIGSTOP"}, // Stop process
            {SIGTSTP, "SIGTSTP"}, // Stop typed at tty
            {SIGTTIN, "SIGTTIN"}, // tty input for background process
            {SIGTTOU, "SIGTTOU"}, // tty output for background process
          }
        );

        return bind[signal];
      }
    } // End of anonymous namespace.
  } // End of signal.

  namespace crash
  {

    Handler::Handler(std::string const& name,
                     bool quit):
      _name{name},
      _quit{quit}
    {}

    Handler::Handler(std::string const& name,
                     bool quit,
                     int argc,
                     char** argv):
      _name{name},
      _quit{quit}
    {
      for (int i = 1; i < argc; ++i)
      {
        if (argv[i] == NULL)
          break;

        this->_name += " ";
        this->_name += argv[i];
      }
    }

    Handler::~Handler()
    {}

    void
    Handler::operator() (boost::system::error_code const& error,
                         int sig)
    {
      if (!error)
      {
        ELLE_DEBUG("signal caught: %s.", elle::signal::strsignal(sig));

        elle::crash::report(this->_name, elle::signal::strsignal(sig));

        if (this->_quit)
          exit(sig);
      }
      else
      {
        ELLE_WARN("Error: %d - Sig: %d", error, sig);
      }
    }

    bool
    report(std::string const& module,
           std::string const& signal,
           reactor::Backtrace const& bt)
    {
      ELLE_TRACE("Report crash");

      std::unique_ptr<elle::HTTPClient> server{
        new elle::HTTPClient{
          common::meta::host(),
          common::meta::port(),
          "InfinitDesktop", // User agent
          false
        }
      };

      elle::format::json::Array bt_arr{}, env_arr{};
      for (auto const& t: bt)
        bt_arr.push_back(static_cast<std::string>(t));

      for (char **env = environ; *env; ++env)
        env_arr.push_back(std::string(*env));

      elle::format::json::Dictionary request;

      request["module"] = module;
      request["signal"] = signal;
      request["backtrace"] = bt_arr;
      request["env"] = env_arr;

      try
        {
          server->put("/debug/report", request);
        }
      catch (...)
        {
          ELLE_WARN("Unable to put on server: '%s'", request.repr());
          return false;
        }

      return true;
    }
  } // End of crash.
} // End of elle.
