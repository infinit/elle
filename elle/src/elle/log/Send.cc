#include <fstream>
#include <mutex>

#include <elle/Exception.hh>
#include <elle/log/Send.hh>
#include <elle/log/SysLogger.hh>
#include <elle/log/TextLogger.hh>
#include <elle/os/environ.hh>
#include <elle/printf.hh>
#include <elle/system/getpid.hh>
#include <elle/types.hh>

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
        auto syslog = elle::os::getenv("ELLE_LOG_SYSLOG", "");
        if (!syslog.empty())
        {
          _logger().reset(new elle::log::SysLogger(
            syslog + "[" + std::to_string(elle::system::getpid()) + "]"));
        }
        else
        {
          auto path = elle::os::getenv("ELLE_LOG_FILE", "");
          bool append = !elle::os::getenv("ELLE_LOG_FILE_APPEND", "").empty();
          if (!path.empty())
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

      bool
      Send::active(elle::log::Logger::Level level,
                   elle::log::Logger::Type,
                   std::string const& component)
      {
        return logger().component_is_active(component, level);
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
          static bool res = elle::os::inenv("ELLE_LOG_CHECK_FORMATS");
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
