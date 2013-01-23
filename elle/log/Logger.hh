#ifndef  ELLE_LOG_LOGGER_HH
# define ELLE_LOG_LOGGER_HH

# include <iosfwd>
# include <memory>
# include <string>
# include <boost/noncopyable.hpp>

namespace elle
{
  namespace log
  {
    class Logger
      : private boost::noncopyable
    {
    public:
      enum class Level
      {
        log = 0,
        trace,
        debug,
        dump,
      };

      enum class Type
      {
        info,
        warning,
        error,
      };

    private:
      std::string   _name;
      std::ostream* _out;

    public:
      Logger(Level lvl,
             std::string const& name = "",
             std::ostream* out = nullptr);
      ~Logger();

    public:
      void message(Level level, elle::log::Logger::Type type, std::string const& values);
      void log(std::string const& msg);
      void trace(std::string const& msg);
      void debug(std::string const& msg);
      void dump(std::string const& msg);

      /// properties
    public:
      std::string const& name() const;
      void name(std::string const& name_);
      void output(std::ostream& out);

    protected:
      void _message(Level level,
                    elle::log::Logger::Type type,
                    std::string const& message);
    };

    /// Here the simplest type possible is used (.rodata-located) so
    /// as to make sure that its initialization will always take place
    /// before the other global variables which construction may require
    /// logging.

    /// @brief Retreive a logger by its name.
    Logger&
    logger(std::string const& name);
  }
}

# include <elle/log/Logger.hxx>

#endif
