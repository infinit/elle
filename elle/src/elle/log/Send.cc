#include <elle/Exception.hh>
#include <elle/log/Send.hh>
#include <elle/log/TextLogger.hh>
#include <elle/printf.hh>
#include <elle/types.hh>

#include <fstream>
#include <mutex>

#include <elle/os/getenv.hh>

namespace elle
{
  namespace log
  {
    std::unique_ptr<Logger>&
    _logger()
    {
      static std::unique_ptr<Logger> logger;
      return logger;
    }

    static
    std::mutex&
    log_mutex()
    {
      static std::mutex mutex;
      return mutex;
    }

    Logger&
    logger()
    {
      std::unique_lock<std::mutex> ulock{log_mutex()};

      if (!_logger())
      {
        std::string path = elle::os::getenv("ELLE_LOG_FILE", "");

        if (path.empty() == false)
        {
          static std::ofstream out{
            path,
              std::fstream::trunc | std::fstream::out
              };
          _logger().reset(new elle::log::TextLogger(out));
        }
        else
          _logger().reset(new elle::log::TextLogger(std::cerr));
      }
      return *_logger();
    }

    void
    logger(std::unique_ptr<Logger> logger)
    {
      ELLE_ASSERT(logger != nullptr);
      std::unique_lock<std::mutex> ulock{log_mutex()};
      if (_logger() != nullptr)
      {
        logger->_indentation = std::move(_logger()->_indentation);
      }
      _logger() = std::move(logger);
    }

    namespace detail
    {
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
        return level <= logger().component_enabled(component);
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
