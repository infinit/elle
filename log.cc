#include <fnmatch.h>

#include <cassert>
#include <cstdlib> // getenv
#include <string>
#include <map>
#include <unordered_map>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/format.hpp>

#include <elle/concurrency/Scheduler.hh>
#include <elle/log.hh>

#include <reactor/storage.hh>

namespace elle
{
  namespace log
  {
    elle::log::Logger default_logger{
        elle::log::Logger::Level::trace
    };

    namespace detail
    {
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
        std::vector<std::string>*     _patterns;
        std::map<std::string, bool>   _enabled;

      public:
        unsigned int                  _max_string_size;
      public:
        unsigned int max_string_size() const
        { return this->_max_string_size; }

      private:
        Components()
          : _patterns(nullptr)
          , _enabled()
          , _max_string_size(0)
        {}

        ~Components()
        {
          delete this->_patterns;
          this->_patterns = nullptr;
        }

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

        bool enabled(std::string const& name) const
        {
          auto it = this->_enabled.find(name);
          if (it == this->_enabled.end())
            throw std::runtime_error("Unknown component name '" + name + "'");
          return it->second;
        }

      private:
        std::vector<std::string> const& patterns()
        {
          if (this->_patterns != nullptr)
            return *this->_patterns;
          this->_patterns = new std::vector<std::string>();

          std::vector<std::string> res;
          static char const* components_str = ::getenv("ELLE_LOG_COMPONENTS");
          if (components_str == nullptr)
              return *this->_patterns;

          boost::algorithm::split(
              res, components_str,
              boost::algorithm::is_any_of(","),
              boost::algorithm::token_compress_on
          );

          for (auto& el: res)
            {
              boost::algorithm::trim(el);
              if (el.size())
                  this->_patterns->push_back(el);
            }
          return *this->_patterns;
        }
      };

      TraceComponent::TraceComponent(std::string const& name_)
        : name(boost::algorithm::trim_copy(name_))
      {
        assert(name.size() && "component name should not be empty");
        Components::instance().enable(this->name);
      }



      TraceContext::TraceContext(TraceComponent const& component)
        : _component(component)
      {
        _indent();
      }

      TraceContext::~TraceContext()
      {
        _unindent();
      }

      void
      TraceContext::_send(char const* file,
                          unsigned int line,
                          char const* function,
                          const std::string& msg)
      {
        static bool const location = ::getenv("ELLE_LOG_LOCATIONS") != nullptr;
        if (location)
          {
            static boost::format fmt("%s:%s: %s (%s)");
            this->_send(str(fmt % file % line % msg % function));
          }
        else
          this->_send(msg);
      }

      static int thread_id(reactor::Thread* t)
      {
        if (!t)
          return 0;

        static int i = 1;
        typedef std::unordered_map<reactor::Thread*, int> Ids;
        static Ids ids;

        auto elt = ids.find(t);
        if (elt == ids.end())
          {
            ids[t] = i;
            return i++;
          }
        else
          return ids[t];
      }


      void
      TraceContext::_send(std::string const& msg)
      {
        if (!Components::instance().enabled(this->_component.name))
          return;
        int indent = _indentation();
        assert(indent >= 1);
        std::string align = std::string((indent - 1) * 2, ' ');
        unsigned int size = this->_component.name.size();
        assert(size <= Components::instance().max_string_size());
        unsigned int pad = Components::instance().max_string_size() - size;
        std::string s = (
          std::string(pad / 2, ' ') +
          this->_component.name +
          std::string(pad / 2 + pad % 2, ' ')
        );

        boost::posix_time::ptime time;
        static const bool universal = ::getenv("ELLE_LOG_TIME_UNIVERSAL") != nullptr;
        if (universal)
          time = boost::posix_time::second_clock::universal_time();
        else
          time = boost::posix_time::second_clock::local_time();
        boost::format fmt("%s: [%s] [%2s]");
        reactor::Thread* t = elle::concurrency::scheduler().current();
        default_logger.trace(str(fmt % time % s % thread_id(t)), align, msg);
      }

      void
      TraceContext::_indent()
      {
        _indentation() += 1;
      }

      void
      TraceContext::_unindent()
      {
        _indentation() -= 1;
      }
    }
  }
}

