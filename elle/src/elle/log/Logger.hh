#ifndef ELLE_LOG_LOGGER_HH
# define ELLE_LOG_LOGGER_HH

# include <memory>
# include <mutex>
# include <string>
# include <unordered_map>
# include <vector>

# include <boost/noncopyable.hpp>

# include <elle/attribute.hh>
# include <elle/memory.hh>
# include <elle/time.hh>

namespace elle
{
  namespace log
  {
    namespace detail
    {
      struct Send;
    }

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
      typedef std::function<std::unique_ptr<Indentation> ()> Factory;
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
      Logger(std::string const& log_level);
      virtual
      ~Logger();
    private:
      void
      _setup_indentation();

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
      std::recursive_mutex _mutex;
      std::unique_ptr<Indentation> _indentation;
      ELLE_ATTRIBUTE_RW(bool, time_universal);
      ELLE_ATTRIBUTE_RW(bool, time_microsec);

    /*----------.
    | Messaging |
    `----------*/
    public:
      void message(Level level,
                   elle::log::Logger::Type type,
                   std::string const& component,
                   std::string const& message,
                   std::string const& file,
                   unsigned int line,
                   std::string const& function);
    protected:
      virtual
      void
      _message(Level level,
               elle::log::Logger::Type type,
               std::string const& component,
               boost::posix_time::ptime const& time,
               std::string const& message,
               std::vector<std::pair<std::string, std::string>> const& tags,
               int indentation,
               std::string const& file,
               unsigned int line,
               std::string const& function) = 0;
      friend class CompositeLogger;

    /*-----------.
    | Components |
    `-----------*/
    public:
      Level
      component_enabled(std::string const& name);
    private:
      /// Rule about components.
      struct Filter
      {
        /// A pattern, e.g., `'infinit.cryptography.*.SecretKey`.
        std::string pattern;
        /// The corrresponding level.
        Level level;

        /// Whether this filter accepts a component name.
        bool
        match(const std::string& component) const;
      };
      std::vector<Filter> _component_patterns;
      std::unordered_map<std::string, Level> _component_levels;
      ELLE_ATTRIBUTE_R(unsigned int, component_max_size);
    };

    ELLE_API
    std::ostream&
    operator << (std::ostream& stream, Logger::Level l);
  }
}

# include <elle/log/Logger.hxx>

#endif
