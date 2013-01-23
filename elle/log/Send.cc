#include <fnmatch.h>
#include <unordered_map>

#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <elle/concurrency/Scheduler.hh>
#include <elle/log/Send.hh>
#include <elle/printf.hh>
#include <elle/reactor/src/reactor/storage.hh>

#include <reactor/exception.hh>

namespace elle
{
  namespace log
  {
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
              throw reactor::Exception(elle::concurrency::scheduler(),
                                       elle::sprintf("invalid log level: %s", level));
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

      Logger&
      logger()
      {
        static Logger logger(default_log_level());
        return logger;
      };

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

      struct Components
      {
      private:
        std::vector<std::string>      _patterns;
        std::map<std::string, bool>   _enabled;

      public:
        unsigned int                  _max_string_size;
      public:
        unsigned int max_string_size() const
        { return this->_max_string_size; }

      private:
        Components()
          : _patterns()
          , _enabled()
          , _max_string_size(0)
        {
          static char const* components_str = ::getenv("ELLE_LOG_COMPONENTS");
          if (components_str == nullptr)
            this->_patterns.push_back("*");
          else
            {
              boost::algorithm::split(this->_patterns, components_str,
                                      boost::algorithm::is_any_of(","),
                                      boost::algorithm::token_compress_on);
              for (auto& pattern: this->_patterns)
                boost::algorithm::trim(pattern);
            }
        }

        ~Components()
        {}

      public:
        static Components& instance()
        {
          static Components* ptr = nullptr;
          if (ptr == nullptr)
            ptr = new Components();
          return *ptr;
        }

        void enable(std::string const& name)
        {
          this->_enabled[name] = false;
          for (auto const& pattern: this->patterns())
            {
              if (fnmatch(pattern.c_str(), name.c_str(), 0) == 0)
                {
                  this->_enabled[name] = true;
                  break;
                }
            }
          if (this->_enabled[name])
            {
              if (name.size() > this->_max_string_size)
                this->_max_string_size = name.size();
            }
        }

        void
        update_max_size(std::string const& name)
        {
          if (name.size() > this->_max_string_size)
            this->_max_string_size = name.size();
        }

        bool
        enabled(std::string const& name) const
        {
          auto it = this->_enabled.find(name);
          if (it == this->_enabled.end())
            {
              const_cast<Components*>(this)->enable(name);
              it = this->_enabled.find(name);
              assert(it != this->_enabled.end());
            }
          return it->second;
        }

      private:
        std::vector<std::string> const& patterns()
        {
          return this->_patterns;
        }
      };

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
        // FIXME: do we always want to print warnings and errors ?
        if (type >= Logger::Type::warning)
          return true;
        if (!Components::instance().enabled(component))
          return false;
        Components::instance().update_max_size(component);
        if (level > default_log_level())
          return false;
        return true;
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
        assert(size <= Components::instance().max_string_size());
        unsigned int pad = Components::instance().max_string_size() - size;
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
