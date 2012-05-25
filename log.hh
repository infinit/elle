#ifndef  ELLE_LOG_HH
# define ELLE_LOG_HH

# include <elle/log/Logger.hh>

namespace elle
{
  namespace log
  {

# define ELLE_LOG_TRACE(...)                                                  \
    ::elle::log::default_logger.trace(                                        \
        __FILE__, __LINE__, ETC_LOG_FUNCTION, ##__VA_ARGS__                   \
    )                                                                         \

    template<typename... T> void debug(T const&...);
    template<typename... T> void info(T const&...);
    template<typename... T> void warn(T const&...);
    template<typename... T> void error(T const&...);
    template<typename... T> void fatal(T const&...);

    extern elle::log::Logger default_logger;
  }
}

# include <elle/log.hxx>

#endif
