#ifndef  ELLE_LOG_LOGGER_HH
# define ELLE_LOG_LOGGER_HH

# include <iostream>
# include <memory>
# include <string>

# include <boost/noncopyable.hpp>

# include <elle/attribute.hh>

namespace elle
{
  namespace log
  {
    class Logger
      : private boost::noncopyable
    {
    /*------.
    | Level |
    `------*/
    public:
      enum class Level
      {
        log = 0,
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
      Logger(Level lvl,
             std::string const& name = "",
             std::ostream& out = std::cerr);
      ~Logger();
      ELLE_ATTRIBUTE_R(std::string, name);

    /*----------.
    | Messaging |
    `----------*/
    public:
      void message(Level level, elle::log::Logger::Type type, std::string const& values);
      void log(std::string const& msg);
      void trace(std::string const& msg);
      void debug(std::string const& msg);
      void dump(std::string const& msg);
    protected:
      void _message(Level level,
                    elle::log::Logger::Type type,
                    std::string const& message);
      ELLE_ATTRIBUTE_R(std::ostream&, output);

    };
  }
}

# include <elle/log/Logger.hxx>

#endif
