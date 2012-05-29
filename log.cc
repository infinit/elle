
#include <fnmatch.h>

#include <cassert>
#include <cstdlib> // getenv
#include <string>
#include <map>
#include <vector>

#include <boost/algorithm/string.hpp>

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
      static ::reactor::LocalStorage<unsigned int> _indent{
          elle::concurrency::scheduler()
      };

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
          char const* components_str = getenv("ELLE_LOG_COMPONENTS");
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
                {
                  this->_patterns->push_back(el);
                  elle::print("\t-", "'"+el+"'");
                }

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
        _indent.Get() += 1;
      }

      TraceContext::~TraceContext()
      {
        _indent.Get() -= 1;
      }

      void TraceContext::_send(std::string const& msg)
      {
        if (!Components::instance().enabled(this->_component.name))
          return;
        int indent = _indentation.Get();
        assert(indent >= 1);
        std::string align = std::string((indent - 1) * 2, ' ');
        unsigned int size = this->_component.name.size();
        assert(size <= Components::instance().max_string_size());
        unsigned int pad = Components::instance().max_string_size() - size;
        std::string s = (
          "[" + std::string(pad / 2, ' ') +
          this->_component.name +
          std::string(pad / 2 + pad % 2, ' ') +
          "]"
        );
        default_logger.trace(s, align, msg);
      }

    }

  }
}

