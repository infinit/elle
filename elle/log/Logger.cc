#include <cassert>

#include <elle/log/Logger.hh>

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

    void
    Logger::_message(Level level,
                     elle::log::Logger::Type type,
                     std::string const& message)
    {
      switch (type)
        {
          case Type::info:
            break;
          case Type::warning:
            // Yellow
            std::cerr << "[33;01;33m";
            break;
          case Type::error:
            // Red
            std::cerr << "[33;01;31m";
        }
      if (level <= _level)
        std::cerr << message << std::endl;
      switch (type)
        {
          case Type::info:
            break;
          case Type::warning:
          case Type::error:
            std::cerr << "[0m";
            break;
        }
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
