#include <fstream>
#include <mutex>

#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/tokenizer.hpp>

#include <elle/Exception.hh>
#include <elle/algorithm.hh>
#include <elle/assert.hh>
#include <elle/log/CompositeLogger.hh>
#include <elle/log/FileLogger.hh>
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
      /// The main logger.
      auto&
      _logger()
      {
        static std::unique_ptr<Logger> logger;
        return logger;
      }

      /// The mutex protecting the main logger.
      auto&
      log_mutex()
      {
        static std::recursive_mutex mutex;
        return mutex;
      }

      /// Split a string around its *second* colon.
      std::tuple<std::string, std::string>
      split(std::string const& s)
      {
        auto const sep = s.find('?');
        if (sep == s.npos)
          return std::make_tuple(s, "LOG");
        else
          return std::make_tuple(s.substr(0, sep), s.substr(sep + 1));
      }


      /// Build a logger from its URL-like specification.
      ///
      /// <DEST>:
      /// `file://cerr`: goes to std::cerr.
      /// `file://NAME`: create file `NAME`.
      /// `file://NAME+`: append to file `NAME`.
      /// `files://BASE`: create `BASE.0`, `BASE.1`, etc.
      /// `syslog://NAME`: to syslog, tagged with `NAME[PID]`.
      std::unique_ptr<Logger>
      make_one_logger(std::string const& t)
      {
        // FIXME: C++17.
        auto const spec = split(t);
        auto const& dest = std::get<0>(spec);
        auto const& level = std::get<1>(spec);
        if (dest == "file://cerr")
          return std::make_unique<TextLogger>(std::cerr, level);
        else if (auto file = elle::tail(dest, "file://"))
        {
          auto const append = boost::ends_with(*file, "+");
          if (append)
            boost::erase_tail(*file, 1);
          auto s = std::make_unique<std::ofstream>(
            *file,
            (append ? std::fstream::app : std::fstream::trunc)
            | std::fstream::out);
          static auto streams = std::vector<std::unique_ptr<std::ofstream>>{};
          streams.emplace_back(std::move(s));
          return std::make_unique<TextLogger>(*streams.back(), level);
        }
        else if (auto s = elle::tail(dest, "files://"))
          return std::make_unique<FileLogger>(*s, level);
        else if (auto s = elle::tail(dest, "syslog://"))
          return std::make_unique<SysLogger>(
            print("%s[%s]", *s, elle::system::getpid()),
            level);
        else
          err("invalid ELLE_LOG_TARGETS: %s", t);
      }
    }

    /// The main logger.  Create it from $ELLE_LOG_TARGETS if it's null.
    ///
    /// In the case of the initialization, we are about to set-up the
    /// main logger, which will shake quite a few pieces of elle, some
    /// of them using, directly or now, the log facility.  As a
    /// consequence `logger()` (this function) will be called several
    /// times, recursively.  So first, we need a recursive_mutex, and
    /// second, set up a temporary TextLogger (so that _logger() is
    /// not null) which will log whatever happens during the call to
    /// `make_logger()`.
    ///
    /// Alternatively, we could use a NullLogger.
    Logger&
    logger()
    {
      std::unique_lock<std::recursive_mutex> ulock{log_mutex()};
      if (!_logger())
      {
        _logger() = std::make_unique<TextLogger>(std::cerr);
        _logger() = make_logger(os::getenv("ELLE_LOG_TARGETS", "file://cerr"));
      }
      return *_logger();
    }

    /// Replace the main logger with another.
    ///
    /// \return the previous logger.
    std::unique_ptr<Logger>
    logger(std::unique_ptr<Logger> logger)
    {
      std::unique_lock<std::recursive_mutex> ulock{log_mutex()};
      if (_logger() && logger)
        logger->_indentation = _logger()->_indentation->clone();
      std::swap(_logger(), logger);
      return logger;
    }

    /// Add a new logger.
    void
    logger_add(std::unique_ptr<Logger> l)
    {
      std::unique_lock<std::recursive_mutex> ulock{log_mutex()};
      auto log = dynamic_cast<CompositeLogger*>(_logger().get());
      if (!log)
      {
        // Create before, so that current logs apply.
        auto clog = std::make_unique<CompositeLogger>();
        auto old = std::move(_logger());
        clog->loggers().push_back(std::move(old));
        log = clog.get();
        _logger() = std::move(clog);
      }
      log->loggers().push_back(std::move(l));
    }

    std::unique_ptr<Logger>
    make_logger(std::string const& targets)
    {
      using Tokenizer = boost::tokenizer<boost::char_separator<char>>;
      auto const sep = boost::char_separator<char>{"; "};
      auto const ts
        = elle::print(targets,
                      {
                        {"pid", elle::system::getpid()},
                      });
      auto loggers
        = elle::make_vector(Tokenizer{ts, sep}, make_one_logger);
      // If there's just one log destination, let's avoid the
      // composite logger.
      if (loggers.size() == 1)
        return std::move(loggers[0]);
      else
        return std::make_unique<CompositeLogger>(std::move(loggers));
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
