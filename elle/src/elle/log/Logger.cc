#ifdef INFINIT_WINDOWS
# include <shlwapi.h>
#else
# include <fnmatch.h>
#endif

#include <functional>
#include <memory>
#include <thread>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread/tss.hpp>

#include <elle/Exception.hh>
#include <elle/Plugin.hh>
#include <elle/log/Logger.hh>
#include <elle/os/environ.hh>
#include <elle/system/getpid.hh>
#include <elle/printf.hh>

namespace elle
{
  namespace log
  {
    /*------------.
    | Indentation |
    `------------*/

    Indentation::~Indentation()
    {}

    class PlainIndentation:
      public Indentation
    {
    public:
      PlainIndentation()
        : _indentation()
      {}

      virtual
      unsigned int&
      indentation()
      {
        if (!this->_indentation.get())
          this->_indentation.reset(new unsigned int(1));
        return *this->_indentation;
      }

      virtual
      void
      indent()
      {
        this->indentation() += 1;
      }

      virtual
      void
      unindent()
      {
        ELLE_ASSERT_GTE(this->indentation(), 1u);
        this->indentation() -= 1;
      }

      virtual
      std::unique_ptr<Indentation>
      clone() override
      {
        auto res = elle::make_unique<PlainIndentation>();
        return res;
      }

    private:
      boost::thread_specific_ptr<unsigned int> _indentation;
    };

    unsigned int&
    Logger::indentation()
    {
      return this->_indentation->indentation();
    }

    void
    Logger::indent()
    {
      this->_indentation->indent();
    }

    void
    Logger::unindent()
    {
      this->_indentation->unindent();
    }

    /*-------------.
    | Construction |
    `-------------*/

    static
    Logger::Level
    parse_level(std::string const& level)
    {
      if (level == "NONE")
        return Logger::Level::none;
      else if (level == "LOG")
        return Logger::Level::log;
      else if (level == "TRACE")
        return Logger::Level::trace;
      else if (level == "DEBUG")
        return Logger::Level::debug;
      else if (level == "DUMP")
        return Logger::Level::dump;
      else
        throw elle::Exception(
          elle::sprintf("invalid log level: %s", level));
  }

    Logger::Logger(std::string const& log_level)
      : _indentation(new PlainIndentation)
      , _time_universal(false)
      , _time_microsec(false)
      , _component_patterns()
      , _component_levels()
      , _component_max_size(0)
    {
      this->_setup_indentation();
      // FIXME: resets indentation
      elle::Plugin<Indenter>::hook_added().connect(
        [this] (Indenter&) { this->_setup_indentation(); });
      std::string levels_str(log_level);
      if (elle::os::inenv("ELLE_LOG_LEVEL"))
        levels_str = elle::os::getenv("ELLE_LOG_LEVEL", "");
      if (!levels_str.empty())
      {
        std::vector<std::string> levels;
        boost::algorithm::split(levels, levels_str,
                                boost::algorithm::is_any_of(","),
                                boost::algorithm::token_compress_on);
        for (auto& pattern: levels)
        {
          auto colon = pattern.find(":");
          Level level = Level::log;
          if (colon != std::string::npos)
          {
            std::string level_str = pattern.substr(colon + 1);
            boost::algorithm::trim(level_str);
            level = parse_level(level_str);
            pattern = pattern.substr(0, colon);
            boost::algorithm::trim(pattern);
          }
          else
          {
            boost::algorithm::trim(pattern);
            level = parse_level(pattern);
            pattern = "*";
          }
          _component_patterns.push_back(std::make_pair(pattern, level));
        }
      }
    }

    Logger::~Logger()
    {}

    void
    Logger::_setup_indentation()
    {
      std::function<std::unique_ptr<Indentation> ()> factory =
        [] () -> std::unique_ptr<Indentation>
        {
          return elle::make_unique<PlainIndentation>();
        };
      for (auto const& indenter: elle::Plugin<Indenter>::plugins())
      {
        factory = [&indenter, factory] ()  -> std::unique_ptr<Indentation>
          {
            return indenter.second->indentation(factory);
          };
      }
      this->_indentation = factory();
    }

    /*----------.
    | Messaging |
    `----------*/

    void
    Logger::message(Level level,
                    elle::log::Logger::Type type,
                    std::string const& component,
                    std::string const& msg,
                    std::string const& file,
                    unsigned int line,
                    std::string const& function)
    {
      std::lock_guard<std::recursive_mutex> lock(_mutex);

      int indent = this->indentation();
      std::vector<std::pair<std::string, std::string>> tags;
      for (auto const& tag: elle::Plugin<Tag>::plugins())
      {
        std::string content = tag.second->content();
        if (!content.empty())
          tags.push_back(std::make_pair(tag.second->name(), content));
      }
      auto time = this->_time_microsec ?
         ( this->_time_universal ?
        boost::posix_time::microsec_clock::universal_time() :
        boost::posix_time::microsec_clock::local_time())
         :  ( this->_time_universal ?
        boost::posix_time::second_clock::universal_time() :
        boost::posix_time::second_clock::local_time());
      if (indent < 1)
      {
        this->_message(
          level, Type::error, component, time,
          elle::sprintf("negative indentation level on log: %s", msg),
          tags, 0, file, line, function);
        std::abort();
      }
      this->_message(level, type, component, time, msg, tags,
                     indent - 1, file, line, function);
    }

    /*--------.
    | Enabled |
    `--------*/

    Logger::Level
    Logger::component_enabled(std::string const& name)
    {
      std::lock_guard<std::recursive_mutex> lock(_mutex);
      auto elt = this->_component_levels.find(name);
      Level res = Level::log;
      if (elt == this->_component_levels.end())
      {
        for (auto const& pattern: this->_component_patterns)
#ifdef INFINIT_WINDOWS
          if (::PathMatchSpec(name.c_str(), pattern.first.c_str()) == TRUE)
#else
          if (fnmatch(pattern.first.c_str(), name.c_str(), 0) == 0)
#endif
            res = pattern.second;

        if (res > Level::none)
          this->_component_max_size =
            std::max(this->_component_max_size,
                     static_cast<unsigned int>(name.size()));
        this->_component_levels[name] = res;
      }
      else
        res = elt->second;
      return res;
    }

    /*------.
    | Level |
    `------*/

    std::ostream&
    operator << (std::ostream& stream, Logger::Level l)
    {
      switch (l)
      {
        case Logger::Level::none:
          stream << "none";
          break;
        case Logger::Level::log:
          stream << "log";
          break;
        case Logger::Level::trace:
          stream << "trace";
          break;
        case Logger::Level::debug:
          stream << "debug";
          break;
        case Logger::Level::dump:
          stream << "dump";
          break;
      }
      return stream;
    }

    /*-----.
    | Tags |
    `-----*/

#define ELLE_LOGGER_TAG(Name, Content)                          \
    class Name##Tag:                                            \
      public elle::log::Tag                                     \
    {                                                           \
    public:                                                     \
      virtual                                                   \
      std::string                                               \
      content()                                                 \
      {                                                         \
        return Content;                                         \
      }                                                         \
                                                                \
      virtual                                                   \
      std::string                                               \
      name()                                                    \
      {                                                         \
        return #Name;                                           \
      }                                                         \
    };                                                          \
                                                                \
    elle::Plugin<Tag>::Register<Name##Tag> register_tag_##Name; \

    ELLE_LOGGER_TAG(
      PID, boost::lexical_cast<std::string>(elle::system::getpid()));
    ELLE_LOGGER_TAG(
      TID, boost::lexical_cast<std::string>(std::this_thread::get_id()));

#undef ELLE_LOGGER_TAG
  }
}
