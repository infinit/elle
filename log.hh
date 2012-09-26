#ifndef ELLE_LOG_HH
# define ELLE_LOG_HH

# include <elle/log/Logger.hh>
# include <elle/assert.hh>
# include <elle/printf.hh>
# include <elle/print.hh>

namespace elle
{
  namespace log
  {
# define ELLE_LOG_COMPONENT(component)                                        \
    static ::elle::log::detail::TraceComponent                                \
    __trace_component__(component)                                            \
/**/

// XXX Move to elle/compiler.hh
# define ELLE_PRETTY_FUNCTION __PRETTY_FUNCTION__


# define ELLE_LOG_LEVEL_SCOPE(Lvl, T, ...)                                    \
    auto BOOST_PP_CAT(__trace_ctx_, __LINE__) =                               \
      ::elle::log::detail::TraceContext                                       \
      (elle::log::Logger::Level::Lvl,                                         \
       elle::log::Logger::Type::T,                                            \
       __trace_component__,                                                   \
       __FILE__, __LINE__, ELLE_PRETTY_FUNCTION,                              \
       elle::sprintf(__VA_ARGS__))                                            \
/**/

# define ELLE_LOG_LEVEL(Lvl, Type, ...)                                       \
    if (ELLE_LOG_LEVEL_SCOPE(Lvl, Type, __VA_ARGS__))                         \
      {                                                                       \
        elle::unreachable();                                                  \
      }                                                                       \
    else                                                                      \
/**/

# define ELLE_LOG_SCOPE(...)   ELLE_LOG_LEVEL_SCOPE(log,   info, __VA_ARGS__)
# define ELLE_TRACE_SCOPE(...) ELLE_LOG_LEVEL_SCOPE(trace, info, __VA_ARGS__)
# define ELLE_DEBUG_SCOPE(...) ELLE_LOG_LEVEL_SCOPE(debug, info, __VA_ARGS__)
# define ELLE_DUMP_SCOPE(...)  ELLE_LOG_LEVEL_SCOPE(dump,  info, __VA_ARGS__)

# define ELLE_WARN(...)  ELLE_LOG_LEVEL(log,   warning, __VA_ARGS__)
# define ELLE_ERR(...)   ELLE_LOG_LEVEL(log,   error,   __VA_ARGS__)
# define ELLE_LOG(...)   ELLE_LOG_LEVEL(log,   info,    __VA_ARGS__)
# define ELLE_TRACE(...) ELLE_LOG_LEVEL(trace, info,    __VA_ARGS__)
# define ELLE_DEBUG(...) ELLE_LOG_LEVEL(debug, info,    __VA_ARGS__)
# define ELLE_DUMP(...)  ELLE_LOG_LEVEL(dump,  info,    __VA_ARGS__)

# include <boost/preprocessor/tuple/size.hpp>
# include <boost/preprocessor/punctuation/comma_if.hpp>

/// Shortcut to trace a function name and its arguments.
///
/// @param  the list of arguments
/// XXX does not work with empty call
# define ELLE_TRACE_FUNCTION(...)                                       \
  ELLE_TRACE_SCOPE("%s(%s)",                                            \
                   __FUNCTION__,                                        \
                   elle::sprint(elle::iomanip::Separator(','),          \
                                ##__VA_ARGS__))

/// Shortcut to trace a method name, the instance and its arguments.
///
/// @param  the list of arguments
/// XXX does not work with empty call
# define ELLE_TRACE_METHOD(...)                                         \
  ELLE_TRACE_SCOPE("#%s %s(%s)",                                        \
                   this, __FUNCTION__,                                  \
                   elle::sprint(elle::iomanip::Separator(','),          \
                                ##__VA_ARGS__))

    namespace detail
    {

      template <typename... Args>
      std::string
      print_function_params(Args const&... args)
      {
        return elle::sprint(elle::iomanip::Separator(", "), args...);
      }

      struct TraceComponent
      {
        std::string const name;
        TraceComponent(std::string const& name);
      };

      struct TraceContext
      {
      public:
        TraceContext(elle::log::Logger::Level level,
                     elle::log::Logger::Type type,
                     TraceComponent const& component,
                     char const* file,
                     unsigned int line,
                     char const* function,
                     std::string const& message);
        ~TraceContext();
        operator bool() const;
      private:
        TraceComponent const& _component;
        void _send(elle::log::Logger::Level level,
                   elle::log::Logger::Type type,
                   char const* file,
                   unsigned int line,
                   char const* function,
                   const std::string& msg);
        void _send(elle::log::Logger::Level level,
                   elle::log::Logger::Type type,
                   std::string const& msg);
        void _indent();
        void _unindent();
      };

    }

  }
}

# include <elle/log.hxx>

#endif
