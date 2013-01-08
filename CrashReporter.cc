#include <fstream>
#include <map>

#include <boost/lexical_cast.hpp>

#include <elle/format/json.hh>
#include <elle/reactor/src/reactor/backtrace.hh>
#include <elle/os/path.hh>
#include <elle/log.hh>

#include <elle/HttpClient.hh>
#include <common/common.hh>

#include "CrashReporter.hh"

ELLE_LOG_COMPONENT("elle.CrashReporter");

namespace elle
{
  bool report_crash(std::string const& module, std::string const& signal = "")
  {
    ELLE_TRACE("Report crash");

    elle::HttpClient server{
      common::meta::host(),
      common::meta::port(),
      false,
    };

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

          server.token(token);
        }
      }
    }

    ELLE_DEBUG("Creating Dictionary");
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

    server.put(destination, request);

    return true;
  }
}
