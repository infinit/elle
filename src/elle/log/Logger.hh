#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/noncopyable.hpp>

#include <elle/attribute.hh>
#include <elle/log/fwd.hh>
#include <elle/memory.hh>
#include <elle/time.hh>

namespace elle
{
  namespace log
  {
    class ELLE_API Indentation
    {
    public:
      virtual
      ~Indentation();
      virtual
      unsigned int&
      indentation() = 0;
      virtual
      void
      indent() = 0;
      virtual
      void
      unindent() = 0;
      virtual
      std::unique_ptr<Indentation>
      clone() = 0;
    };

    class ELLE_API Indenter
    {
    public:
      using Factory = std::function<std::unique_ptr<Indentation> ()>;
      virtual
      std::unique_ptr<Indentation>
      indentation(Factory const& factory) = 0;
    };

    class ELLE_API Tag
    {
    public:
      virtual
      std::string
      name() = 0;
      virtual
      std::string
      content() = 0;
    };

    using Tags = std::vector<std::pair<std::string, std::string>>;

    class ELLE_API Logger
      : private boost::noncopyable
    {
    /*------.
    | Level |
    `------*/
    public:
      enum class Level
      {
        none,
        log,
        trace,
        debug,
        dump,
      };

    /*-----.
    | Type |
    `-----*/
    public:
      enum class Type
      {
        info,
        warning,
        error,
      };

    /*-------------.
    | Construction |
    `-------------*/
    public:
      /// Create a logger.
      ///
      /// @param log_level  default level.  Overriden by $ELLE_LOG_LEVEL.
      Logger(std::string const& log_level);
      virtual
      ~Logger();
    private:
      void
      _setup_indentation();

      /// Process a string formatted like $ELLE_LOG_LEVEL.
      void
      _setup_levels(std::string const& levels);

    /*------------.
    | Indentation |
    `------------*/
    public:
      friend struct detail::Send;
      friend
      std::unique_ptr<Logger>
      logger(std::unique_ptr<Logger> logger);
      unsigned int&
      indentation();
      void
      indent();
      void
      unindent();
    private:
      mutable std::recursive_mutex _mutex;
      std::unique_ptr<Indentation> _indentation;
      ELLE_ATTRIBUTE_RW(bool, time_universal);
      ELLE_ATTRIBUTE_RW(bool, time_microsec);

    /*----------.
    | Messaging |
    `----------*/
    public:
      /// Send a log message.
      ///
      /// @param level   the verbosity level
      /// @param type    the severity
      /// @param component  the name of the component, e.g. "elle.Log"
      /// @param message  the actually log message
      /// @param file    the source file from which comes the message
      /// @param line    and its line number
      /// @param function and the name of the calling function
      void message(Level level,
                   Type type,
                   std::string const& component,
                   std::string const& message,
                   std::string const& file,
                   unsigned int line,
                   std::string const& function);
    protected:
      using Tags = std::vector<std::pair<std::string, std::string>>;
      using Time = boost::posix_time::ptime;
      virtual
      void
      _message(Level level,
               Type type,
               std::string const& component,
               Time const& time,
               std::string const& message,
               Tags const& tags,
               int indentation,
               std::string const& file,
               unsigned int line,
               std::string const& function) = 0;
      friend class CompositeLogger;

    /*-----------.
    | Components |
    `-----------*/
    public:
      /// Whether participates as a context or as a component for this
      /// level.
      bool
      component_is_active(std::string const& name, Level level = Level::log);

      /// Log level in the current context of components.
      Level
      component_level(std::string const& name);

      /// Push this component in the component stack.
      void
      component_push(std::string const& name);

      /// Pop the top component from the component stack.
      void
      component_pop();

    private:
      /// Nested components.
      ///
      /// Yes, a stack of (copies) of strings.  Cannot use
      /// reference_wrapper, as there is no guarantee that the
      /// component names outlive us.
      using component_stack_t = std::vector<std::string>;

      /// Rule about components.
      struct Filter
      {
        Filter(std::string c, std::string p, Level l)
          : context{std::move(c)}
          , pattern{std::move(p)}
          , level{l}
        {}

        /// Whether this filter accepts this component name.
        bool
        match(std::string const& component) const;

        /// Whether this filter accepts this component stack
        bool
        match(component_stack_t const& stack) const;

        /// Whether this filter accepts this component name in this component stack.
        bool
        match(std::string const& component,
              component_stack_t const& stack) const;

        /// A context pattern denoting the requested nesting
        /// component, e.g., `infinit.model.*.ACB`.
        std::string context;
        /// A component pattern, e.g., `infinit.cryptography.*.SecretKey`.
        std::string pattern;
        /// The corrresponding level.
        Level level;
      };

      /// Translation of $ELLE_LOG_LEVEL into ordered filters.
      std::vector<Filter> _component_patterns;
      /// A cache of the decoding of $ELLE_LOG_LEVEL: component-name
      /// => Level.  Filled only for unconditional levels (i.e., when
      /// $ELLE_LOG_LEVEL uses no context specification).
      std::unordered_map<std::string, Level> _component_levels;
      ELLE_ATTRIBUTE_R(unsigned int, component_max_size);
      /// Nested components.
      ELLE_ATTRIBUTE_R(component_stack_t, component_stack);
    };

    ELLE_API
    std::ostream&
    operator << (std::ostream& stream, Logger::Level l);
  }
}
