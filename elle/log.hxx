#ifndef  ELLE_LOG_HXX
# define ELLE_LOG_HXX

# include <elle/printf.hh>

namespace elle
{
  namespace log
  {

#define ELLE_LOG_LEVEL_MESSAGE(Lvl)                             \
    inline                                                      \
    void                                                        \
    Lvl(std::string const& values)                              \
    {                                                           \
      return default_logger.Lvl(values);                        \
    }                                                           \

    ELLE_LOG_LEVEL_MESSAGE(log);
    ELLE_LOG_LEVEL_MESSAGE(trace);
    ELLE_LOG_LEVEL_MESSAGE(debug);
    ELLE_LOG_LEVEL_MESSAGE(dump);
#undef ELLE_LOG_LEVEL_MESSAGE

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
        this->send(file, line, function, values...);
      }


      template<typename... T>
      bool TraceContext::send(char const* file,
                              unsigned int line,
                              char const* function,
                              T const&... values)
      {
        this->_send(file, line, function, elle::sprintf(values...));
        return false;
      }

    }

  }
}

#endif
