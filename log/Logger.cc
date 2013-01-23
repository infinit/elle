#include <cassert>
#include <unistd.h>

#include <elle/concurrency/Scheduler.hh>
#include <elle/log/Logger.hh>
#include <elle/printf.hh>

#include <reactor/exception.hh>

namespace elle
{
  namespace log
  {
    void
    Logger::message(Level level,
                    elle::log::Logger::Type type,
                    std::string const& msg)
    {
      this->_message(level, type, msg);
    }

#define ELLE_LOG_LEVEL_MESSAGE(Lvl)                     \
    void                                                \
    Logger::Lvl(std::string const& msg)                 \
    {                                                   \
    return this->message(Logger::Level::Lvl,            \
                         Logger::Type::info,            \
                         msg);                          \
    }                                                   \

    ELLE_LOG_LEVEL_MESSAGE(log);
    ELLE_LOG_LEVEL_MESSAGE(trace);
    ELLE_LOG_LEVEL_MESSAGE(debug);
    ELLE_LOG_LEVEL_MESSAGE(dump);
#undef ELLE_LOG_LEVEL_MESSAGE

    Logger::Logger(Logger::Level level,
                   std::string const& name,
                   std::ostream& out)
      : _name(name)
      , _output(out)
    {}

    Logger::~Logger()
    {}

    static
    bool
    color()
    {
      static const char* color_env_name = "ELLE_LOG_COLOR";
      char* color_env = ::getenv(color_env_name);
      if (color_env)
        {
          std::string c(color_env);
          if (c == "0")
            return false;
          else if (c == "1")
            return true;
          else
            throw reactor::Exception(elle::concurrency::scheduler(),
                                     elle::sprintf("invalid value for %s: %s",
                                                   color_env_name, c));
        }
      else
        return isatty(2);
    }

    void
    Logger::_message(Level level,
                     elle::log::Logger::Type type,
                     std::string const& message)
    {
      static bool c = color();
      std::string color_code;
      if (c)
        switch (type)
          {
            case Type::info:
              break;
            case Type::warning:
              // Yellow
              color_code =  "[33;01;33m";
              break;
            case Type::error:
              // Red
              color_code = "[33;01;31m";
              break;
          }
      _output << color_code;
      _output << message << std::endl;
      if (!color_code.empty())
        _output << "[0m";
      _output.flush();
    }
  }
}
