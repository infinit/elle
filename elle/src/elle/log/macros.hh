#ifndef ELLE_LOG_MACROS_HH

# define ELLE_LOG_MACROS_HH
# define ELLE_LOG_COMPONENT(_component_)                                \
    static char const* _trace_component_ = _component_;

# define ELLE_LOG_LEVEL_SCOPE(Lvl, T, ...)                              \
    auto BOOST_PP_CAT(__trace_ctx_, __LINE__) =                         \
      ::elle::log::detail::Send                                         \
      (elle::log::Logger::Level::Lvl,                                   \
       elle::log::Logger::Type::T,                                      \
       _trace_component_,                                               \
       __FILE__, __LINE__, ELLE_COMPILER_PRETTY_FUNCTION,               \
       __VA_ARGS__)                                                     \

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

/// Shortcuts to log a function name and its arguments.
///
/// @param  the list of arguments
/// XXX does not work with empty call

# define ELLE_TRACE_FUNCTION(...)                                       \
  ELLE_TRACE_SCOPE("%s(%s)",                                            \
                   __FUNCTION__,                                        \
                   elle::sprint(elle::iomanip::Separator(", "),         \
                                ##__VA_ARGS__))

# define ELLE_DEBUG_FUNCTION(...)                                       \
  ELLE_DEBUG_SCOPE("%s(%s)",                                            \
                   __FUNCTION__,                                        \
                   elle::sprint(elle::iomanip::Separator(", "),         \
                                ##__VA_ARGS__))

# define ELLE_DUMP_FUNCTION(...)                                        \
  ELLE_DUMP_SCOPE("%s(%s)",                                             \
                  __FUNCTION__,                                         \
                  elle::sprint(elle::iomanip::Separator(", "),          \
                               ##__VA_ARGS__))

/// Shortcut to log a method name, the instance and its arguments.
///
/// @param  the list of arguments
/// XXX does not work with empty call
# define ELLE_TRACE_METHOD(...)                                         \
  ELLE_TRACE_SCOPE("%s(%s) <%s at %s>",                                 \
                   __FUNCTION__,                                        \
                   elle::sprint(elle::iomanip::Separator(", "),         \
                                ##__VA_ARGS__),                         \
                   *this, this)

# define ELLE_DEBUG_METHOD(...)                                         \
  ELLE_DEBUG_SCOPE("%s(%s) <%s at %s>",                                 \
                   __FUNCTION__,                                        \
                   elle::sprint(elle::iomanip::Separator(", "),         \
                                ##__VA_ARGS__),                         \
                   *this, this)

# define ELLE_DUMP_METHOD(...)                                          \
  ELLE_DUMP_SCOPE("%s(%s) <%s at %s>",                                  \
                  __FUNCTION__,                                         \
                  elle::sprint(elle::iomanip::Separator(", "),          \
                               ##__VA_ARGS__),                          \
                  *this, this)

# include <elle/log/Send.hh>

#endif
