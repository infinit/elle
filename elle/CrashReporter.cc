#include <fstream>
#include <map>
#include <signal.h>

#include <elle/format/json.hh>
#include <elle/concurrency/Scheduler.hh>
#include <elle/reactor/src/reactor/backtrace.hh>
#include <elle/os/path.hh>
#include <elle/log.hh>

#include <elle/HttpClient.hh>
#include <common/common.hh>

#include "CrashReporter.hh"

ELLE_LOG_COMPONENT("elle.CrashReporter");

namespace elle
{
  namespace signal
  {
    ScoppedGuard::ScoppedGuard()
      : _signals(elle::concurrency::scheduler().io_service())
    {
    }

    ScoppedGuard::ScoppedGuard(std::vector<int> const& sig,
                               Handler const& handler)
      : ScoppedGuard{}
    {
      this->init(sig, handler);
    }

    void
    ScoppedGuard::init(std::vector<int> const& sig,
                       Handler const& handler)
    {
      if (sig.size() == 0)
        return;

      std::for_each(sig.begin(), sig.end(), [&](int sig) { this->_signals.add(sig); });

      this->_signals.async_wait(handler);
    }

    void
    ScoppedGuard::release()
    {
      this->_signals.clear();
    }

    namespace
    {
      std::string const&
      strsignal(int signal)
      {
        static std::map<int, std::string> bind(
          {
            {SIGHUP,  "SIGHUP"},
            {SIGINT,  "SIGINT"},
            {SIGILL,  "SIGILL"},
            {SIGABRT, "SIGABRT"},
            {SIGFPE,  "SIGFPE"},
            {SIGKILL, "SIGKILL"},
            {SIGSEGV, "SIGSEGV"},
            {SIGPIPE, "SIGPIPE"},
            {SIGALRM, "SIGALRM"},
            {SIGTERM, "SIGTERM"},
            {SIGCHLD, "SIGCHLD"},
            {SIGCONT, "SIGCONT"},
            {SIGSTOP, "SIGSTOP"},
            {SIGTSTP, "SIGTSTP"},
            {SIGTTIN, "SIGTTIN"},
            {SIGTTOU, "SIGTTOU"},
          }
        );

        return bind[signal];
      }
    }
  }

  namespace crash
  {
    void
    report_handler(boost::system::error_code const& error,
                   int sig)
    {
      if (!error)
      {
        ELLE_DEBUG("signal caught: %s.", elle::signal::strsignal(sig));

        elle::crash::report("castor", elle::signal::strsignal(sig));
      }
      else
      {
        ELLE_LOG("Error: %d", error);
      }
    }

    void
    exiting_report_handler(boost::system::error_code const& error,
                           int sig)
    {
      report_handler(error, sig);

      exit(sig);
    }

    bool
    report(std::string const& module, std::string const& signal)
    {
      ELLE_TRACE("Report crash");

      std::unique_ptr<elle::HttpClient> server;
      try
      {
        server.reset(new elle::HttpClient{
          common::meta::host(),
          common::meta::port(),
          false,
        });
      }
      catch(...) //XXX
      {
        ELLE_WARN("Unable to reach server.");
        return false;
      }

      std::string user, destination = "/debug/report";

      // If the user has already logged, use the "normal" way to send the report.
      if (user.length() > 0)
      {
        std::string identity_path = common::watchdog::identity_path(user);

        if (identity_path.length() > 0 && elle::os::path::exists(identity_path))
        {
          std::ifstream identity;
          identity.open(identity_path);

          if (identity.good())
          {
            std::string token;
            std::getline(identity, token);

            server->token(token);
          }
        }
      }

      elle::format::json::Dictionary request{
        std::map<std::string, std::string>{
          {"module", module},
        }
      };

      elle::format::json::Array bt_arr{}, env_arr{};

      for (auto const& t: reactor::Backtrace::current())
      {
        bt_arr.push_back(static_cast<std::string>(t));
      }
      for (char **env = environ; *env; ++env)
      {
        env_arr.push_back(std::string(*env));
      }

      request["module"] = module;
      request["signal"] = signal;
      request["backtrace"] = bt_arr;
      request["env"] = env_arr;

      try
      {
        server->put(destination, request);
      }
      catch(...)
      {
        ELLE_WARN("Unable to put on server.");
        return false;
      }

      return true;
    }
  }
}
