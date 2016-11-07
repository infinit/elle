#ifndef ELLE_LOG_MACROS_HH
# define ELLE_LOG_MACROS_HH

# ifdef ELLE_LOG_DISABLE

#  define ELLE_LOG_COMPONENT(_component_) /* */
#  define ELLE_LOG_LEVEL_SCOPE(Lvl, T, ...) /* */
#  define ELLE_LOG_LEVEL(Lvl, T, ...) /* */

# else

#  define ELLE_LOG_COMPONENT(_component_)                               \
  static char const* __attribute__((unused))                            \
    _trace_component_ = _component_;                                    \

#  define ELLE_LOG_VALUE(Lvl, T, ...)                                   \
    [&] {                                                               \
       static bool active =                                             \
         elle::log::detail::Send::active(elle::log::Logger::Level::Lvl, \
                                         elle::log::Logger::Type::T,    \
                                         _trace_component_);            \
        return active;}()  ?                                            \
      ::elle::log::detail::Send                                         \
      (elle::log::Logger::Level::Lvl,                                   \
       elle::log::Logger::Type::T,                                      \
       true,                                                            \
       _trace_component_,                                               \
       __FILE__, __LINE__, ELLE_COMPILER_PRETTY_FUNCTION,               \
       __VA_ARGS__) : ::elle::log::detail::Send()

#  define ELLE_LOG_LEVEL_SCOPE(Lvl, T, ...)                             \
    auto BOOST_PP_CAT(__trace_ctx_, __LINE__) =                         \
      ELLE_LOG_VALUE(Lvl, T, __VA_ARGS__)                               \

#  define ELLE_LOG_LEVEL(Lvl, Type, ...)                                \
  if (ELLE_LOG_LEVEL_SCOPE(Lvl, Type, __VA_ARGS__))                     \
  {                                                                     \
    elle::unreachable();                                                \
  }                                                                     \
  else                                                                  \
/**/
# endif

# define ELLE_LOG_SCOPE(...)   ELLE_LOG_LEVEL_SCOPE(log,   info, __VA_ARGS__)
# define ELLE_TRACE_SCOPE(...) ELLE_LOG_LEVEL_SCOPE(trace, info, __VA_ARGS__)
# define ELLE_WARN_SCOPE(...)  ELLE_LOG_LEVEL_SCOPE(log,   warning, __VA_ARGS__)
# define ELLE_DEBUG_SCOPE(...) ELLE_LOG_LEVEL_SCOPE(debug, info, __VA_ARGS__)
# define ELLE_DUMP_SCOPE(...)  ELLE_LOG_LEVEL_SCOPE(dump,  info, __VA_ARGS__)

# define ELLE_WARN(...)  ELLE_LOG_LEVEL(log,   warning, __VA_ARGS__)
# define ELLE_ERR(...)   ELLE_LOG_LEVEL(log,   error,   __VA_ARGS__)
# define ELLE_LOG(...)   ELLE_LOG_LEVEL(log,   info,    __VA_ARGS__)
# define ELLE_TRACE(...) ELLE_LOG_LEVEL(trace, info,    __VA_ARGS__)
# define ELLE_DEBUG(...) ELLE_LOG_LEVEL(debug, info,    __VA_ARGS__)
# define ELLE_DUMP(...)  ELLE_LOG_LEVEL(dump,  info,    __VA_ARGS__)

# include <elle/log/Send.hh>

#endif
