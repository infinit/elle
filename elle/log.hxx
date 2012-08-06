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
      inline
      TraceContext::operator bool() const
      {
        return false;
      }
    }

  }
}

#endif
