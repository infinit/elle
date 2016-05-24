#include <elle/Exception.hh>
#include <elle/log/Send.hh>
#include <elle/log/TextLogger.hh>
#include <elle/printf.hh>
#include <elle/types.hh>

#include <fstream>
#include <mutex>

#include <elle/os/environ.hh>

namespace elle
{
  namespace log
  {
    static
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

    std::unique_ptr<Logger>
    logger(std::unique_ptr<Logger> logger)
    {
      std::unique_lock<std::mutex> ulock{log_mutex()};
      if (_logger() != nullptr && logger != nullptr)
        logger->_indentation = _logger()->_indentation->clone();
      std::unique_ptr<Logger> prev = std::move(_logger());
      _logger() = std::move(logger);
      return prev;
    }

    namespace detail
    {
      Send::~Send()
      {
        if (!_proceed)
          return;
        this->_unindent();
      }

      bool
      Send::_enabled(elle::log::Logger::Type,
                     elle::log::Logger::Level level,
                     elle::String const& component)
      {
        return level <= logger().component_enabled(component);
      }

      void
      Send::_send(elle::log::Logger::Level level,
                  elle::log::Logger::Type type,
                  bool indent,
                  std::string const& component,
                  char const* file,
                  unsigned int line,
                  char const* function,
                  const std::string& msg)
      {
        logger().message(level, type, component, msg, file, line, function);
        if (indent)
          this->_indent();
      }

      /*------------.
      | Indentation |
      `------------*/

      void
      Send::_indent()
      {
        this->_indentation = &logger().indentation();
        ++*this->_indentation;
      }

      void
      Send::_unindent()
      {
        if (this->_indentation)
        {
          --*this->_indentation;
          this->_indentation = nullptr;
        }
      }
    }
  }
}
