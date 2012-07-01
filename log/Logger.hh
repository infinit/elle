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
        trace = 0,
        debug,
        info,
        warn,
        error,
        fatal,

        _max_value,
      };

    private:
      struct Impl;
      Impl* _impl;

    public:
      Logger(Level lvl, std::string const& name = "");
      ~Logger();

    public:
      template<typename... T> void message(Level level, T const&... values);
      template<typename... T> void trace(T const&... values);
      template<typename... T> void debug(T const&... values);
      template<typename... T> void info(T const&... values);
      template<typename... T> void warn(T const&... values);
      template<typename... T> void error(T const&... values);
      template<typename... T> void fatal(T const&... values);

      /// properties
    public:
      std::string const& name() const;
      void name(std::string const& name_);

      Level level() const;
      void level(Level level_);

      /// Set the out stream for a specific level
      void stream(Level level, std::ostream& out);
      void stream(Level level, std::unique_ptr<std::ostream>&& out);

      /// Set the out stream for all level
      void stream(std::ostream& out);

    protected:
      void _message(Level level, std::string const& message);
    };

  }
}

# include <elle/log/Logger.hxx>

#endif /* ! LOGGER_HH */
