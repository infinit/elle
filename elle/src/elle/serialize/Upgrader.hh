#ifndef  ELLE_SERIALIZE_UPGRADER_HH
# define ELLE_SERIALIZE_UPGRADER_HH

# include "StaticFormat.hh"
# include "StoreFormat.hh"

namespace elle
{
  namespace serialize
  {

    template <typename T, unsigned int format, typename... Args>
    struct Upgrader
    {
      static inline
      void
      upgrade(T&, Args&...)
      { /* format == value.format */ }
    };

  }
}
///
/// @brief Declare an upgrade method.
///
/// @param __type       The type that will be upgradable.
/// @param __format     The format number to upgrade to.
/// @param __value      The name of the value to be upgraded.
/// @param __VA_ARGS__  Names of additional parameters required to upgrade.
///
/// The upgrade process from a format i to another format j will always go
/// through all format number in between. You can assume that @a __value is at
/// format (__format - 1).
///
/// Despite the fact that you can define an upgrade method that take any kind
/// of parameters, you will encounter compile errors if you try to define
/// multiple upgraders for the same type with different parameter types.
///
/// WARNING: You have to define every steps (from format 1 to the current
/// format) to upgrade a type, even if there is nothing to do in one step.
///
# define ELLE_SERIALIZE_UPGRADE(__type, __format, __value, ...)               \
  _ELLE_SERIALIZE_UPGRADE(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__),                \
                          __type,                                             \
                          __format,                                           \
                          __value,                                            \
                          ##__VA_ARGS__)                                      \
/**/

# define _ELLE_SERIALIZE_UPGRADE(__n, __type, __format, __value, ...)         \
namespace elle                                                                \
{                                                                             \
  namespace serialize                                                         \
  {                                                                           \
    template <__ESU_TEMPLATE_ARGS_DEF(__n)>                                   \
    struct Upgrader<__type, __format, __ESU_TEMPLATE_ARGS(__n)>               \
    {                                                                         \
      static_assert(StaticFormat<T>::version <= __format,                     \
                    "Upgrade format is greater than the current one");        \
      static_assert(StoreFormat<T>::value == true,                            \
                    "Cannot define an upgrader for a non-versionned type");   \
      template <typename... Args>                                             \
      static inline                                                           \
      void                                                                    \
      upgrade(__type& val, uint16_t to, Args&... args)                        \
      {                                                                       \
        if (__format > to)                                                    \
          return;                                                             \
        if (val.version() <= __format)                                        \
          _upgrade(val, args...);                                             \
        Upgrader<__type, __format + 1, Args...>::upgrade(val, args...);       \
      }                                                                       \
    private:                                                                  \
      static inline                                                           \
      void                                                                    \
      _upgrade(__type& __value                                                \
               BOOST_PP_COMMA_IF(__n)                                         \
              __ESU_TEMPLATE_PARAMS(&, __VA_ARGS__));                         \
    };                                                                        \
  }                                                                           \
}                                                                             \
template <__ESU_TEMPLATE_ARGS_DEF(__n)>                                       \
void                                                                          \
Upgrader<__type, __format, Args...>::_upgrade(                                \
    __type& __value                                                           \
    BOOST_PP_COMMA_IF(__n)                                                    \
    __ESU_TEMPLATE_PARAMS(&, __VA_ARGS__)                                     \
)                                                                             \
/**/

//- internal macros -----------------------------------------------------------

# include <boost/preprocessor/cat.hpp>
# include <boost/preprocessor/control/if.hpp>
# include <boost/preprocessor/facilities/empty.hpp>
# include <boost/preprocessor/punctuation/comma_if.hpp>
# include <boost/preprocessor/repetition/repeat.hpp>
# include <boost/preprocessor/seq/for_each_i.hpp>
# include <boost/preprocessor/variadic/size.hpp>
# include <boost/preprocessor/variadic/to_seq.hpp>

#define __ESU_TYPENAME_REPEAT(z, n, prefix)                                   \
  BOOST_PP_COMMA_IF(n) BOOST_PP_CAT(prefix T, BOOST_PP_INC(n))                \
/**/

// T0, ..., TN
#define __ESU_TEMPLATE_ARGS(n)                                                \
  BOOST_PP_REPEAT(n, __ESU_TYPENAME_REPEAT, BOOST_PP_EMPTY())                 \
/**/

// typename T0, ..., typename TN
#define __ESU_TEMPLATE_ARGS_DEF(n)                                            \
  BOOST_PP_REPEAT(n, __ESU_TYPENAME_REPEAT, typename)                         \
/**/

# define __ESU_TEMPLATE_PARAMS_REPEAT(r, suffix, i, data)                     \
  BOOST_PP_COMMA_IF(i) data suffix                                            \
/**/

// __ESU_TEMPLATE_PARAMS(const, pif, paf, pouf)
//    -> T0 const pif, T1 const paf, T2 const pouf
# define __ESU_TEMPLATE_PARAMS(suffix, ...)                                   \
  BOOST_PP_FOR_EACH_I(__ESU_TEMPLATE_PARAMS_REPEAT,                           \
                      suffix,                                                 \
                      BOOST_PP_VARIADIC_TO_SEQ(...))                          \
/**/

#endif
