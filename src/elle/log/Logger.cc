#include <fnmatch.h>

#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <elle/log/Logger.hh>
#include <elle/os/getenv.hh>
#include <elle/printf.hh>

namespace elle
{
  namespace log
  {
    /*------------.
    | Indentation |
    `------------*/

    class PlainIndentation: public Indentation
    {
    public:
      PlainIndentation()
        : _indentation(0)
      {}

      virtual
      unsigned int
      indentation()
      {
        return this->_indentation;
      }

      virtual
      void
      indent()
      {
        this->_indentation += 1;
      }

      virtual
      void
      unindent()
      {
        assert(_indentation >= 1);
        this->_indentation -= 1;
      }

    private:
      unsigned int _indentation;
    };

    std::function<std::unique_ptr<Indentation> ()>&
    Logger::_factory()
    {
      static std::function<std::unique_ptr<Indentation> ()> res =
        [] () { return std::unique_ptr<Indentation>(new PlainIndentation()); };
      return res;
    }

    unsigned int
    Logger::indentation()
    {
      std::lock_guard<std::mutex> lock(_indentation_mutex);
      return this->_indentation->indentation();
    }

    void
    Logger::indent()
    {
      std::lock_guard<std::mutex> lock(_indentation_mutex);
      this->_indentation->indent();
    }

    void
    Logger::unindent()
    {
      std::lock_guard<std::mutex> lock(_indentation_mutex);
      this->_indentation->unindent();
    }

    /*----.
    | Tag |
    `----*/

    std::vector<std::unique_ptr<Tag>>&
    Logger::_tags()
    {
      static std::vector<std::unique_ptr<Tag>> res;
      return res;
    }

    /*-------------.
    | Construction |
    `-------------*/

    Logger::Logger()
      : _enable_pid(elle::os::getenv("ELLE_LOG_PID", "") == "1")
      , _enable_time(elle::os::getenv("ELLE_LOG_TIME", "") == "1")
      , _universal_time(elle::os::getenv("ELLE_LOG_TIME_UNIVERSAL", "") == "1")
      , _indentation(this->_factory()())
      , _component_patterns()
      , _component_enabled()
      , _component_max_size(0)
    {
      std::string components_str = elle::os::getenv("ELLE_LOG_COMPONENTS", "");
      if (components_str == "")
        this->_component_patterns.push_back("*");
      else
        {
          boost::algorithm::split(this->_component_patterns, components_str,
                                  boost::algorithm::is_any_of(","),
                                  boost::algorithm::token_compress_on);
          for (auto& pattern: this->_component_patterns)
            boost::algorithm::trim(pattern);
        }
    }

    Logger::~Logger()
    {}

    /*----------.
    | Messaging |
    `----------*/

    void
    Logger::message(Level level,
                    elle::log::Logger::Type type,
                    std::string const& component,
                    std::string const& msg)
    {
      int indent = this->indentation();
      assert(indent >= 1);

      // Component
      std::string comp;
      {
        unsigned int size = component.size();
        assert(size <= this->component_max_size());
        unsigned int pad = this->component_max_size() - size;
        comp = std::string(pad / 2, ' ')
          + component + std::string(pad / 2 + pad % 2, ' ');
      }

      // Time
      boost::posix_time::ptime ptime;
      {
        if (_universal_time)
          ptime = boost::posix_time::second_clock::universal_time();
        else
          ptime = boost::posix_time::second_clock::local_time();
      }

      // PID
      std::string pid;
      {
        pid = boost::lexical_cast<std::string>(getpid());
      }

      // Message
      std::string message;
      {
        std::string align = std::string((indent - 1) * 2, ' ');
        message = align + msg;
      }

      // Format
      std::string prefix;
      if (_enable_time)
        prefix += elle::sprintf("%s: ", ptime);
      if (_enable_pid)
        prefix += elle::sprintf("[%s] ", pid);
      prefix += elle::sprintf("[%s] ", comp);
      for (auto& tag: this->_tags())
        {
          std::string content(tag->content());
          if (!content.empty())
            prefix += elle::sprintf("[%s] ", content);
        }
      this->_message(level, type, prefix + message);
    }

    /*--------.
    | Enabled |
    `--------*/

    bool
    Logger::component_enabled(std::string const& name)
    {
      auto elt = this->_component_enabled.find(name);
      if (elt == this->_component_enabled.end())
        {
          bool res = false;
          for (auto const& pattern: this->_component_patterns)
            {
              if (fnmatch(pattern.c_str(), name.c_str(), 0) == 0)
                {
                  this->_component_max_size =
                    std::max(this->_component_max_size,
                             static_cast<unsigned int>(name.size()));
                  res = true;
                  break;
                }
            }
          this->_component_enabled[name] = res;
          return res;
        }
      else
        return elt->second;
    }
  }
}
