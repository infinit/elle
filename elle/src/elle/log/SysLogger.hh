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
      _message(Level level,
               elle::log::Logger::Type type,
               std::string const& component,
               boost::posix_time::ptime const& time,
               std::string const& message,
               std::vector<std::pair<std::string, std::string>> const& tags,
               int indentation,
               std::string const& file,
               unsigned int line,
               std::string const& function) override;
    };
  }
}
