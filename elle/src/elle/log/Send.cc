#include <elle/Exception.hh>
#include <elle/log/Send.hh>
#include <elle/log/TextLogger.hh>
#include <elle/log/SysLogger.hh>
#include <elle/system/getpid.hh>
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
        auto syslog = elle::os::getenv("ELLE_LOG_SYSLOG", "");
        if (!syslog.empty())
        {
          _logger().reset(new elle::log::SysLogger(
            syslog + "[" + std::to_string(elle::system::getpid()) + "]"));
        }
        else
        {
          std::string path = elle::os::getenv("ELLE_LOG_FILE", "");
          bool append = !elle::os::getenv("ELLE_LOG_FILE_APPEND", "").empty();
          if (path.empty() == false)
          {
            static std::ofstream out{
              path,
                (append ? std::fstream::app : std::fstream::trunc)
                  | std::fstream::out
                };
            _logger().reset(new elle::log::TextLogger(out));
          }
          else
            _logger().reset(new elle::log::TextLogger(std::cerr));
        }
      }
      return *_logger();
    }

    void
    logger(std::unique_ptr<Logger> logger)
    {
      std::unique_lock<std::mutex> ulock{log_mutex()};
      if (_logger() != nullptr && logger != nullptr)
        logger->_indentation = std::move(_logger()->_indentation);
      _logger() = std::move(logger);
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
