#include <fstream>
#include <mutex>

#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>

#include <elle/Exception.hh>
#include <elle/algorithm.hh>
#include <elle/assert.hh>
#include <elle/bytes.hh>
#include <elle/find.hh>
#include <elle/from-string.hh>
#include <elle/log/CompositeLogger.hh>
#include <elle/log/FileLogger.hh>
#include <elle/log/Send.hh>
#include <elle/log/SysLogger.hh>
#include <elle/log/TextLogger.hh>
#include <elle/make-vector.hh>
#include <elle/os/environ.hh>
#include <elle/printf.hh>
#include <elle/system/getpid.hh>

using namespace std::literals;

ELLE_LOG_COMPONENT("elle.log");

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

      /// Split a string one a given separator.
      auto
      tokenize(std::string const& s, char const* sep)
      {
        using Tokenizer = boost::tokenizer<boost::char_separator<char>>;
        auto const csep = boost::char_separator<char>{sep};
        return Tokenizer{s, csep};
      }

      /// Split a string around its last question mark.
      ///
      /// On the last one so that `?` is valid in file names:
      /// `file:///tmp/wtf????LOG` sends `LOG` to `/tmp/wtf???`.
      std::tuple<std::string, std::string>
      split(std::string s)
      {
        boost::trim(s);
        auto const sep = s.rfind('?');
        if (sep == s.npos)
          return std::make_tuple(s, "LOG");
        else
          return std::make_tuple(s.substr(0, sep), s.substr(sep + 1));
      }

      /// Whether a logger spec is a log level (as opposed to an
      /// argument such as `append`, or `size=100KiB`).
      bool
      is_log_level(std::string const& s)
      {
        return (s == "NONE"
                || s == "LOG"
                || s == "TRACE"
                || s == "DEBUG"
                || s == "DUMP"
                || s.find(':') != s.npos);
      }

      using Arguments = std::map<std::string, std::string>;

      /// Get the value of type `T` under the name `k`.
      template <typename T = bool>
      T
      get(Arguments& args, std::string const& k, T def = false)
      {
        if (auto i = elle::find(args, k))
        {
          auto res = elle::from_string<T>(std::move(i->second));
          args.erase(i);
          return res;
        }
        else
          return def;
      }

      std::unique_ptr<Logger>
      make_one_logger_impl(std::string const& dest,
                           std::string const& level,
                           Arguments& args)
      {
        if (auto file = elle::tail(dest, "file://"))
        {
          auto const append = get(args, "append", false);
          auto const envvar = get(args, "var", "ELLE_LOG_LEVEL"s);
          auto const size = convert_capacity(get(args, "size", "0b"s));
          auto const rotate = get(args, "rotate", 0);
          return std::make_unique<FileLogger>(*file, level,
                                              size, rotate, append, envvar);
        }
        else if (dest == "stderr://")
          return std::make_unique<TextLogger>(std::cerr, level);
        else if (auto s = elle::tail(dest, "syslog://"))
          return std::make_unique<SysLogger>(
            print("%s[%s]", *s, elle::system::getpid()),
            level);
        else
          err<std::invalid_argument>("invalid log destination: %s", dest);
      }


      /// Build a logger from its URL-like specification.
      std::unique_ptr<Logger>
      make_one_logger(std::string const& t)
      {
        ELLE_LOG_COMPONENT("elle.log");
        // FIXME: C++17.
        auto const dest_specs = split(t);
        auto const& dest = std::get<0>(dest_specs);
        auto const& specs = std::get<1>(dest_specs);
        // Iterate on the various specs of the level, separating
        // arguments of the logger from log_level.
        auto args = Arguments{};
        auto level = ""s;
        {
          for (auto const& t: tokenize(specs, ","))
            if (is_log_level(t))
              level += (level.empty() ? "" : ",") + t;
            else
            {
              auto pos = t.find('=');
              // Not emplace, so that `foo=false,foo=true` stores true.
              if (pos == t.npos)
                args[t] = "true";
              else
                args[t.substr(0, pos)] = t.substr(pos+1);
            }
        }
        auto res = make_one_logger_impl(dest, level, args);
        if (auto l = get_text_logger(res))
        {
          l->enable_time(get(args, "time",
                             os::getenv("ELLE_LOG_TIME", false)));
          l->time_universal(get(args, "universal",
                                os::getenv("ELLE_LOG_TIME_UNIVERSAL", false)));
          l->time_microsec(get(args, "microsec",
                               os::getenv("ELLE_LOG_TIME_MICROSEC", false)));
        }
        if (!args.empty())
          err<std::invalid_argument>("unused logger arguments: %s", args);
        return res;
      }
    }

    /// Get the TextLogger from a TextLogger or a FileLogger.
    TextLogger*
    get_text_logger(std::unique_ptr<Logger>& l)
    {
      if (auto const res = dynamic_cast<TextLogger*>(l.get()))
        return res;
      else if (auto const flog = dynamic_cast<FileLogger*>(l.get()))
        return dynamic_cast<TextLogger*>(flog->logger().get());
      else
        return nullptr;
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
        auto const level = os::getenv("ELLE_LOG_LEVEL", "LOG"s);
        _logger() = std::make_unique<TextLogger>(std::cerr, level);
        auto const ts = os::getenv("ELLE_LOG_TARGETS", "stderr://?" + level);
        _logger() = make_logger(ts);
        ELLE_DUMP("built main logger: {}", ts);
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
      ELLE_DUMP("add new logger: {}", l);
      auto log = dynamic_cast<CompositeLogger*>(_logger().get());
      if (!log)
      {
        // Create before, so that current logs apply.
        auto clog = std::make_unique<CompositeLogger>();
        clog->loggers().push_back(std::move(_logger()));
        log = clog.get();
        _logger() = std::move(clog);
      }
      log->loggers().push_back(std::move(l));
    }

    std::unique_ptr<Logger>
    make_logger(std::string const& targets)
    {
      auto const ts
        // On Windows we might get a path that uses `\` as a
        // separator.  Escape it for elle::print.
        = elle::print(boost::replace_all_copy(targets, "\\", "\\\\"),
                      {
                        {"pid", elle::system::getpid()},
                      });
      auto loggers = elle::make_vector(tokenize(ts, ";"), make_one_logger);
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
