#include <syslog.h>

#include <elle/log/SysLogger.hh>
#include <elle/printf.hh>

namespace elle
{
  namespace log
  {
    SysLogger::SysLogger(std::string const& name)
    {
      openlog(strdup(name.c_str()), 0, LOG_DAEMON);
    }

    void
    SysLogger::_message(Level level,
                        Logger::Type type,
                        std::string const& component,
                        boost::posix_time::ptime const& time,
                        std::string const& message,
                        std::vector<std::pair<std::string, std::string>> const& tags,
                        int indentation,
                        std::string const& file,
                        unsigned int line,
                        std::string const& function)
    {
      int lvl = 0;
      switch (type)
      {
        case Logger::Type::error:
          lvl = LOG_ERR;
          break;
        case Logger::Type::info:
          switch (level)
          {
            case Logger::Level::none:
            case Logger::Level::log:
              lvl = LOG_INFO;
              break;
            case Logger::Level::trace:
            case Logger::Level::debug:
            case Logger::Level::dump:
              lvl = LOG_DEBUG;
              break;
          }
          break;
        case Logger::Type::warning:
          lvl = LOG_WARNING;
          break;
      }
      std::string msg;
      msg += elle::sprintf("[%s] ", component);
      for (auto const& tag: tags)
        msg += elle::sprintf("[%s]", tag.second);
      msg += std::string(indentation * 2 + 1, ' ');
      msg += message;
      syslog(lvl, "%s", msg.c_str());
    }
  }
}
