#ifdef INFINIT_WINDOWS
# include <shlwapi.h>
#else
# include <fnmatch.h>
#endif

#include <functional>
#include <iterator> // cbegin, etc.
#include <memory>
#include <regex>
#include <thread>

#include <boost/lexical_cast.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <boost/thread/tss.hpp>
#include <boost/tokenizer.hpp>

#include <elle/Exception.hh>
#include <elle/Plugin.hh>
#include <elle/assert.hh>
#include <elle/log/Logger.hh>
#include <elle/os/environ.hh>
#include <elle/printf.hh>
#include <elle/system/getpid.hh>

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

      unsigned int&
      indentation() override
      {
        if (!this->_indentation.get())
          this->_indentation.reset(new unsigned int(1));
        return *this->_indentation;
      }

      void
      indent() override
      {
        this->indentation() += 1;
      }

      void
      unindent() override
      {
        ELLE_ASSERT_GTE(this->indentation(), 1u);
        this->indentation() -= 1;
      }

      std::unique_ptr<Indentation>
      clone() override
      {
        return std::make_unique<PlainIndentation>();
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
      : _indentation(std::make_unique<PlainIndentation>())
      , _time_universal(false)
      , _time_microsec(false)
      , _component_max_size(0)
    {
      this->_setup_indentation();
      // FIXME: resets indentation
      elle::Plugin<Indenter>::hook_added().connect(
        [this] (Indenter&) { this->_setup_indentation(); }
      );
      auto levels = elle::os::getenv("ELLE_LOG_LEVEL", log_level);
      if (!levels.empty())
        this->_setup_levels(levels);
    }

    Logger::~Logger()
    {}

    void
    Logger::_setup_indentation()
    {
      std::function<std::unique_ptr<Indentation> ()> factory =
        [] () -> std::unique_ptr<Indentation>
        {
          return std::make_unique<PlainIndentation>();
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

    void
    Logger::_setup_levels(const std::string& levels)
    {
      using tokenizer = boost::tokenizer<boost::char_separator<char>>;
      auto sep = boost::char_separator<char>{","};
      for (auto& level: tokenizer{levels, sep})
      {
        static auto re =
          std::regex{" *"
                     "(?:(.*)  *)?"     // 1: context
                     "(?:"
                     "([^ :]*)"         // 2: component
                     " *: *"
                     ")?"
                     "([^ :]*)"         // 3: level
                     " *"};

        auto m = std::smatch{};
        if (std::regex_match(level, m, re))
          this->_component_patterns
            .emplace_back(m[1],
                          m[2].length() ? m[2].str() : "*",
                          parse_level(m[3]));
        else
          throw elle::Exception(
            elle::sprintf("invalid level specification: %s", level));
      }
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

      if (!this->component_is_active(component, level))
        return;

      int indent = this->indentation();
      auto tags = std::vector<std::pair<std::string, std::string>>{};
      for (auto const& tag: elle::Plugin<Tag>::plugins())
      {
        std::string content = tag.second->content();
        if (!content.empty())
          tags.emplace_back(tag.second->name(), content);
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

    static
    bool
    _fnmatch(const std::string& pattern, const std::string& s)
    {
#ifdef INFINIT_WINDOWS
      return ::PathMatchSpec(s.c_str(), pattern.c_str()) == TRUE;
#else
      return fnmatch(pattern.c_str(), s.c_str(), 0) == 0;
#endif
    }

    bool
    Logger::Filter::match(const std::string& s) const
    {
      return _fnmatch(pattern, s);
    }

    bool
    Logger::Filter::match(const component_stack_t& stack) const
    {
      // Either there is no request on the context, or some component
      // matches it.
      return
        (context.empty()
         || (boost::find_if(stack,
                           [this](const auto& comp)
                           {
                             return _fnmatch(context, comp);
                           })
             != stack.cend()));
    }

    bool
    Logger::Filter::match(const std::string& s,
                          const component_stack_t& stack) const
    {
      return this->match(s) && this->match(stack);
    }

    bool
    Logger::component_is_active(const std::string& name,
                                Level level)
    {
      std::lock_guard<std::recursive_mutex> lock(_mutex);
      auto res = level <= this->component_level(name);
      // Update the max width of displayed component names.
      if (res)
        this->_component_max_size =
          std::max(this->_component_max_size,
                   static_cast<unsigned int>(name.size()));
      return res;
    }

    Logger::Level
    Logger::component_level(std::string const& name)
    {
      std::lock_guard<std::recursive_mutex> lock(_mutex);
      auto res = Level::log;

      auto i = this->_component_levels.find(name);
      if (i == this->_component_levels.cend())
      {
        for (auto const& filter: this->_component_patterns)
          if (filter.match(name))
          {
            if (filter.match(this->_component_stack))
              res = filter.level;
            // If enabled unconditionally, cache it.
            if (filter.context.empty())
              // Several filters might apply (e.g.,
              // $ELLE_LOG_LEVEL="LOG,DUMP"), keep the last one.
              this->_component_levels[name] = res;
          }
      }
      else
        res = i->second;
      return res;
    }

    Logger::Level
    Logger::component_enabled(std::string const& name)
    {
      return this->component_level(name);
    }

    /*-------------.
    | Components.  |
    `-------------*/

    void
    Logger::component_push(std::string const& name)
    {
      this->_component_stack.emplace_back(name);
    }

    void
    Logger::component_pop()
    {
      assert(!this->_component_stack.empty());
      this->_component_stack.pop_back();
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
