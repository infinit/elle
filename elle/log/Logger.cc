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

    Logger::Logger(Logger::Level level, std::string const& name)
      : _level(level)
      , _name(name)
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
      std::cerr << color_code;
      if (level <= _level)
        std::cerr << message << std::endl;
      if (!color_code.empty())
        std::cerr << "[0m";
    }

    std::string const&
    Logger::name() const
    {
      return this->_name;
    }

    void
    Logger::name(std::string const& name_)
    {
      this->_name = name_;
    }

    Logger::Level
    Logger::level() const
    {
      return this->_level;
    }

    void
    Logger::level(Level level_)
    {
      this->_level = level_;
    }

  }
}
