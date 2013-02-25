#include <elle/Exception.hh>
#include <elle/log/Send.hh>
#include <elle/log/TextLogger.hh>
#include <elle/printf.hh>
#include <elle/types.hh>

namespace elle
{
  namespace log
  {
    std::unique_ptr<Logger>&
    _logger()
    {
      static std::unique_ptr<Logger> logger;
      return logger;
    };

    Logger&
    logger()
    {
      if (!_logger())
        _logger().reset(new elle::log::TextLogger(std::cerr));
      return *_logger();
    };

    void
    logger(std::unique_ptr<Logger> logger)
    {
      _logger() = std::move(logger);
    };

    namespace detail
    {
      static
      Logger::Level
      _default_log_level()
      {
        static const char* env = ::getenv("ELLE_LOG_LEVEL");
        if (env)
          {
            const std::string level(env);
            if (level == "LOG")
              return Logger::Level::log;
            else if (level == "TRACE")
              return Logger::Level::trace;
            else if (level == "DEBUG")
              return Logger::Level::debug;
            else if (level == "DUMP")
              return Logger::Level::dump;
            else
              throw elle::Exception
                (elle::sprintf("invalid log level: %s", level));
          }
        else
          return Logger::Level::log;
      }

      Logger::Level
      default_log_level()
      {
        static Logger::Level level = _default_log_level();
        return level;
      }

      Send::~Send()
      {
        if (!_proceed)
          return;
        logger().unindent();
      }

      bool
      Send::_enabled(elle::log::Logger::Type type,
                     elle::log::Logger::Level level,
                     elle::String const& component)
      {
        return logger().component_enabled(component) &&
          level <= default_log_level();
      }

      void
      Send::_send(elle::log::Logger::Level level,
                          elle::log::Logger::Type type,
                          std::string const& component,
                          char const* file,
                          unsigned int line,
                          char const* function,
                          const std::string& msg)
      {
        static bool const location = ::getenv("ELLE_LOG_LOCATIONS") != nullptr;
        logger().indent();
        if (location)
          {
            static boost::format fmt("%s (at %s:%s in %s)");
            this->_send(level, type, component,
                        str(fmt % msg % file % line % function));
          }
        else
          this->_send(level, type, component, msg);
      }

      void
      Send::_send(elle::log::Logger::Level level,
                          elle::log::Logger::Type type,
                          std::string const& component,
                          std::string const& msg)
      {
        logger().message(level, type, component, msg);
      }
    }
  }
}
