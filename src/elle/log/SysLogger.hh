#pragma once

#include <elle/log/Logger.hh>

namespace elle
{
  namespace log
  {
    class SysLogger: public Logger
    {
    public:
      SysLogger(std::string const& name,
                std::string const& log_level = "",
                bool universal_time = false);
    protected:
      void
      _message(Message const& msg) override;
    };
  }
}
