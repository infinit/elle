#ifndef  ELLE_LOG_LOGGER_HH
# define ELLE_LOG_LOGGER_HH

# include <elle/idiom/Close.hh>
#  include <iosfwd>
#  include <memory>
#  include <string>
#  include <boost/noncopyable.hpp>

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
      Level       _level;
      std::string _name;

    public:
      Logger(Level lvl, std::string const& name = "");
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

      Level level() const;
      void level(Level level_);

    protected:
      void _message(Level level,
                    elle::log::Logger::Type type,
                    std::string const& message);
    };

  }
}

# include <elle/log/Logger.hxx>

#endif
