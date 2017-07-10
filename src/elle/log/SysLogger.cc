#include <elle/assert.hh>
#include <elle/syslog.hh>
#include <elle/log/SysLogger.hh>
#include <elle/print.hh>

namespace elle
{
  namespace log
  {
    SysLogger::SysLogger(std::string name,
                         std::string const& log_level,
                         bool universal_time)
      : Logger{log_level}
      , _name{std::move(name)}
    {
      this->time_universal(universal_time);
      openlog(this->_name.c_str(), 0, LOG_DAEMON);
    }

    void
    SysLogger::_message(Message const& msg)
    {
      int lvl = [type = msg.type, level = msg.level]{
        switch (type)
        {
          case Type::error:
            return LOG_ERR;
          case Type::info:
            switch (level)
            {
              case Level::none:
              case Level::log:
                return LOG_INFO;
              case Level::trace:
              case Level::debug:
              case Level::dump:
                return LOG_DEBUG;
            }
          case Type::warning:
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
