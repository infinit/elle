#include <fstream>
#include <mutex>

#include <elle/Exception.hh>
#include <elle/log/Send.hh>
#include <elle/log/SysLogger.hh>
#include <elle/log/TextLogger.hh>
#include <elle/os/environ.hh>
#include <elle/printf.hh>
#include <elle/system/getpid.hh>

namespace elle
{
  namespace log
  {
    namespace
    {
      std::unique_ptr<Logger>&
      _logger()
      {
        static std::unique_ptr<Logger> logger;
        return logger;
      }

      std::mutex&
      log_mutex()
      {
        static std::mutex mutex;
        return mutex;
      }
    }

    Logger&
    logger()
    {
      std::unique_lock<std::mutex> ulock{log_mutex()};

      if (!_logger())
      {
        // ELLE_LOG_SYSLOG: the name of the logs.
        auto const syslog = elle::os::getenv("ELLE_LOG_SYSLOG", "");
        if (!syslog.empty())
          _logger() = std::make_unique<SysLogger>(
            elle::sprintf("%s[%s]", syslog, elle::system::getpid()));
        else
        {
          auto const path = elle::os::getenv("ELLE_LOG_FILE", "");
          if (path.empty())
            _logger() = std::make_unique<TextLogger>(std::cerr);
          else
          {
            bool const append = elle::os::getenv("ELLE_LOG_FILE_APPEND", false);
            static std::ofstream out{
              path,
              (append ? std::fstream::app : std::fstream::trunc)
              | std::fstream::out
            };
            _logger() = std::make_unique<TextLogger>(out);
          }
        }
      }
      return *_logger();
    }

    std::unique_ptr<Logger>
    logger(std::unique_ptr<Logger> logger)
    {
      std::unique_lock<std::mutex> ulock{log_mutex()};
      if (_logger() && logger)
        logger->_indentation = _logger()->_indentation->clone();
      std::swap(_logger(), logger);
      return logger;
    }

    namespace detail
    {
      bool
      Send::active(Logger::Level level,
                   Logger::Type,
                   std::string const& component)
      {
        return logger().component_is_active(component, level);
      }

      void
      Send::_send(Logger::Level level,
                  Logger::Type type,
                  bool indent,
                  std::string const& component,
                  char const* file,
                  unsigned int line,
                  char const* function,
                  const std::string& msg)
      {
        logger().message(level, type, component, msg, file, line, function);
        if (indent)
          this->_indent(component);
      }

      /*------------.
      | Indentation |
      `------------*/

      void
      Send::_indent(std::string const& component)
      {
        auto& l = logger();
        l.component_push(component);
        this->_indentation = &l.indentation();
        ++*this->_indentation;
      }

      void
      Send::_unindent()
      {
        if (this->_indentation)
        {
          logger().component_pop();
          --*this->_indentation;
          this->_indentation = nullptr;
        }
      }

      namespace
      {
        bool&
        _debug_formats()
        {
          static bool res = elle::os::getenv("ELLE_LOG_CHECK_FORMATS", false);
          return res;
        }
      }

      bool
      debug_formats()
      {
        return _debug_formats();
      }

      void
      debug_formats(bool v)
      {
        _debug_formats() = v;
      }
    }
  }
}
