#include <elle/assert.hh>
#include <elle/syslog.hh>
#include <elle/log/SysLogger.hh>
#include <elle/print.hh>

namespace elle
{
  namespace log
  {
    SysLogger::SysLogger(std::string const& name,
                         std::string const& log_level,
                         bool universal_time)
      : Logger(log_level)
    {
      this->time_universal(universal_time);
      openlog(strdup(name.c_str()), 0, LOG_DAEMON);
    }

    void
    SysLogger::_message(Message const& msg)
    {
      int lvl = [type = msg.type, level = msg.level]{
        switch (type)
        {
          case Logger::Type::error:
            return LOG_ERR;
          case Logger::Type::info:
            switch (level)
            {
              case Logger::Level::none:
              case Logger::Level::log:
                return LOG_INFO;
              case Logger::Level::trace:
              case Logger::Level::debug:
              case Logger::Level::dump:
                return LOG_DEBUG;
            }
          case Logger::Type::warning:
            return LOG_WARNING;
        }
        unreachable();
      }();
      auto str = print("[%s] ", msg.component);
      for (auto const& tag: msg.tags)
        str += print("[%s]", tag.second);
      str += std::string(msg.indentation * 2 + 1, ' ');
      str += msg.message;
      syslog(lvl, "%s", str.c_str());
    }
  }
}
