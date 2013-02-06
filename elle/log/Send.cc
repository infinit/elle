#include <boost/date_time/posix_time/posix_time.hpp>

#include <elle/types.hh>
#include <elle/concurrency/Scheduler.hh>
#include <elle/log/Send.hh>
#include <elle/log/TextLogger.hh>
#include <elle/printf.hh>
#include <elle/reactor/src/reactor/storage.hh>

#include <reactor/exception.hh>

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
              throw reactor::Exception
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

      static
      boost::mutex&
      _indentation_mutex()
      {
        static boost::mutex mutex;
        return mutex;
      }

      static unsigned int&
      _indentation()
      {
        typedef ::reactor::LocalStorage<unsigned int> Storage;
        static Storage indentation(elle::concurrency::scheduler());
        return indentation.Get(0);
      }

      Send::~Send()
      {
        if (!_proceed)
          return;
        _unindent();
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
        if (location)
          {
            static boost::format fmt("%s (at %s:%s in %s)");
            this->_send(level, type, component,
                        str(fmt % file % line % msg % function));
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
        int indent;
        {
          boost::lock_guard<boost::mutex> lock(_indentation_mutex());
          indent = _indentation();
        }
        assert(indent >= 1);
        std::string align = std::string((indent - 1) * 2, ' ');
        unsigned int size = component.size();
        assert(size <= logger().component_max_size());
        unsigned int pad = logger().component_max_size() - size;
        std::string s = (
          std::string(pad / 2, ' ') +
          component +
          std::string(pad / 2 + pad % 2, ' ')
        );

        boost::posix_time::ptime ptime;
        static const bool time =
          ::getenv("ELLE_LOG_TIME") != nullptr;
        static boost::format model(time ?
                                   "%s: [%s] [%s] %s%s" :
                                   "[%s] [%s] %s%s");
        if (time)
          {
            static const bool universal =
              ::getenv("ELLE_LOG_TIME_UNIVERSAL") != nullptr;
            if (universal)
              ptime = boost::posix_time::second_clock::universal_time();
            else
              ptime = boost::posix_time::second_clock::local_time();
          }
        boost::format fmt(model);
        reactor::Thread* t = elle::concurrency::scheduler().current();
        if (time)
          fmt % ptime % s % (t ? t->name() : std::string(" ")) % align % msg;
        else
          fmt % s % (t ? t->name() : std::string(" ")) % align % msg;
        std::string pid = "[" + std::to_string(getpid()) + "]";
        logger().message(level, type, pid + str(fmt));
      }

      void
      Send::_indent()
      {
        boost::lock_guard<boost::mutex> lock(_indentation_mutex());
        _indentation() += 1;
      }

      void
      Send::_unindent()
      {
        boost::lock_guard<boost::mutex> lock(_indentation_mutex());
        auto& indentation = _indentation();
        assert(indentation >= 1);
        indentation -= 1;
      }
    }
  }
}
