#ifndef  ELLE_LOG_HH
# define ELLE_LOG_HH

# include <elle/log/Logger.hh>

namespace elle
{
  namespace log
  {

    template<typename... T> void debug(T const&...);
    template<typename... T> void info(T const&...);
    template<typename... T> void warn(T const&...);
    template<typename... T> void error(T const&...);
    template<typename... T> void fatal(T const&...);

    extern elle::log::Logger default_logger;

# define ELLE_LOG_TRACE_COMPONENT(component)                                  \
    static ::elle::log::detail::TraceComponent                                \
        __trace_component__(component)                                        \

# define ETC_LOG_FUNCTION __PRETTY_FUNCTION__

#define ELLE_LOG_TRACE_SCOPE(...)                                       \
    auto BOOST_PP_CAT(__trace_ctx_, __LINE__) =                         \
      ::elle::log::detail::TraceContext                                 \
      (__trace_component__,                                             \
       __FILE__, __LINE__, ETC_LOG_FUNCTION,                            \
       ##__VA_ARGS__)                                                   \

# define ELLE_LOG_TRACE(...)                                            \
    if (ELLE_LOG_TRACE_SCOPE(__VA_ARGS__))                              \
      { /* Nothing */ }                                                 \
    else                                                                \


    namespace detail
    {

      struct TraceComponent
      {
        std::string const name;
        TraceComponent(std::string const& name);
      };

      struct TraceContext
      {
      public:
        TraceContext(TraceComponent const& component);
        template<typename... T>
        TraceContext(TraceComponent const& component,
                     char const* file,
                     unsigned int line,
                     char const* function,
                     T const&... values);
        ~TraceContext();
        template<typename... T>
        bool
        send(char const* file,
             unsigned int line,
             char const* function,
             T const&... values);
        operator bool() const { return false;}
      private:
        TraceComponent const& _component;
        void _send(char const* file,
                   unsigned int line,
                   char const* function,
                   const std::string& msg);
        void _send(std::string const& msg);
        void _indent();
        void _unindent();
      };

    }

  }
}

# include <elle/log.hxx>

#endif
