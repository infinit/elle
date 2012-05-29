#ifndef  ELLE_LOG_HXX
# define ELLE_LOG_HXX

# include <elle/format.hh>
# include <elle/log.hh>

namespace elle
{
  namespace log
  {


    template<typename... T> void debug(T const&... values)
    {
      return default_logger.debug(values...);
    }

    template<typename... T> void info(T const&... values)
    {
      return default_logger.info(values...);
    }

    template<typename... T> void warn(T const&... values)
    {
      return default_logger.warn(values...);
    }

    template<typename... T> void error(T const&... values)
    {
      return default_logger.error(values...);
    }

    template<typename... T> void fatal(T const&... values)
    {
      return default_logger.fatal(values...);
    }

    namespace detail
    {
      template<typename... T>
      TraceContext::TraceContext(TraceComponent const& component,
                                 char const* file,
                                 unsigned int line,
                                 char const* function,
                                 T const&... values)
        : _component(component)
      {
        _indent();
        send(file, line, function, values...);
      }


      template<typename... T>
      bool TraceContext::send(char const* file,
                              unsigned int line,
                              char const* function,
                              T const&... values)
      {
        this->_send(file, line, function, elle::format(values...));
        return false;
      }

    }

  }
}

#endif
