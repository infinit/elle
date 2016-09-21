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

#  define ELLE_LOG_LEVEL_SCOPE(Lvl, T, ...)                             \
    auto BOOST_PP_CAT(__trace_ctx_, __LINE__) =                         \
     [&] {                                                              \
       static bool enabled =                                            \
         elle::log::detail::Send::_enabled(elle::log::Logger::Type::T,  \
                                        elle::log::Logger::Level::Lvl,  \
                                        _trace_component_);             \
        return enabled;}()  ?                                           \
      ::elle::log::detail::Send                                         \
      (elle::log::Logger::Level::Lvl,                                   \
       elle::log::Logger::Type::T,                                      \
       true,                                                            \
       _trace_component_,                                               \
       __FILE__, __LINE__, ELLE_COMPILER_PRETTY_FUNCTION,               \
       __VA_ARGS__) :  ::elle::log::detail::Send()

#  define ELLE_LOG_LEVEL(Lvl, T, ...)   \
    if ([&] { static bool enabled =         \
        elle::log::detail::Send::_enabled(elle::log::Logger::Type::T,    \
                                        elle::log::Logger::Level::Lvl,   \
                                        _trace_component_);             \
    return enabled;}() &&  ::elle::log::detail::Send                    \
      (elle::log::Logger::Level::Lvl,                                   \
       elle::log::Logger::Type::T,                                      \
       true,                                                            \
       _trace_component_,                                               \
       __FILE__, __LINE__, ELLE_COMPILER_PRETTY_FUNCTION,               \
       __VA_ARGS__))                                                     \
      {elle::unreachable();}                                             \
      else
/*
#  define ELLE_LOG_LEVEL(Lvl, Type, ...)                                      \
    if (ELLE_LOG_LEVEL_SCOPE(Lvl, Type, __VA_ARGS__))                         \
      {                                                                       \
        elle::unreachable();                                                  \
      }                                                                       \
    else                                                                      \
    */
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

# include <boost/preprocessor/tuple/size.hpp>
# include <boost/preprocessor/punctuation/comma_if.hpp>

/// Shortcuts to log a function name and its arguments.
///
/// @param  the list of arguments
/// XXX does not work with empty call

# ifdef ELLE_LOG_DISABLE
#  define ELLE_TRACE_FUNCTION(...) /* */
#  define ELLE_DEBUG_FUNCTION(...) /* */
#  define ELLE_TRACE_METHOD(...) /* */
#  define ELLE_DEBUG_METHOD(...) /* */

# else
#  define ELLE_TRACE_FUNCTION(...)                                            \
  std::string BOOST_PP_CAT(__trace_str_, __LINE__);                           \
  if (elle::log::detail::Send::_enabled(elle::log::Logger::Type::info,        \
                                        elle::log::Logger::Level::trace,      \
                                        _trace_component_))                   \
    BOOST_PP_CAT(__trace_str_, __LINE__) =                                    \
      elle::sprint(elle::iomanip::Separator(", "), ##__VA_ARGS__);            \
  ELLE_TRACE_SCOPE("%s(%s)",                                                  \
                   __FUNCTION__,                                              \
                   BOOST_PP_CAT(__trace_str_, __LINE__))                      \
/**/

#  define ELLE_DEBUG_FUNCTION(...)                                            \
  std::string BOOST_PP_CAT(__trace_str_, __LINE__);                           \
  if (elle::log::detail::Send::_enabled(elle::log::Logger::Type::info,        \
                                        elle::log::Logger::Level::debug,      \
                                        _trace_component_))                   \
    BOOST_PP_CAT(__trace_str_, __LINE__) =                                    \
      elle::sprint(elle::iomanip::Separator(", "), ##__VA_ARGS__);            \
  ELLE_DEBUG_SCOPE("%s(%s)",                                                  \
                   __FUNCTION__,                                              \
                   BOOST_PP_CAT(__trace_str_, __LINE__))                      \
/**/

/// Shortcut to log a method name, the instance and its arguments.
///
/// @param  the list of arguments
/// XXX does not work with empty call

#  define ELLE_TRACE_METHOD(...)                                              \
  std::string BOOST_PP_CAT(__trace_str_, __LINE__);                           \
  if (elle::log::detail::Send::_enabled(elle::log::Logger::Type::info,        \
                                        elle::log::Logger::Level::trace,      \
                                        _trace_component_))                   \
    BOOST_PP_CAT(__trace_str_, __LINE__) =                                    \
      elle::sprint(elle::iomanip::Separator(", "), ##__VA_ARGS__);            \
  ELLE_TRACE_SCOPE("%s(%s) <%s at %s>",                                       \
                   __FUNCTION__,                                              \
                   BOOST_PP_CAT(__trace_str_, __LINE__), *this, this)         \
/**/

#  define ELLE_DEBUG_METHOD(...)                                              \
  std::string BOOST_PP_CAT(__trace_str_, __LINE__);                           \
  if (elle::log::detail::Send::_enabled(elle::log::Logger::Type::info,        \
                                        elle::log::Logger::Level::debug,      \
                                        _trace_component_))                   \
    BOOST_PP_CAT(__trace_str_, __LINE__) =                                    \
      elle::sprint(elle::iomanip::Separator(", "), ##__VA_ARGS__);            \
  ELLE_DEBUG_SCOPE("%s(%s) <%s at %s>",                                       \
                   __FUNCTION__,                                              \
                   BOOST_PP_CAT(__trace_str_, __LINE__), *this, this)         \
/**/

# endif
# include <elle/log/Send.hh>

#endif
