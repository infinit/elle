#ifndef  ELLE_SERIALIZE_WRAPPER_HH
# define ELLE_SERIALIZE_WRAPPER_HH

# include <elle/serialize/fwd.hh>

# define _ELLE_SERIALIZE_WRAPPER(__name, __lower_name)                        \
namespace elle                                                                \
{                                                                             \
  namespace serialize                                                         \
  {                                                                           \
                                                                              \
    template <typename T>                                                     \
    struct __name                                                             \
    {                                                                         \
      T& value;                                                               \
                                                                              \
      explicit __name(T& value): value(value) {}                              \
      __name(__name&&) = default;                                             \
      __name(__name const&) = default;                                        \
      __name& operator =(__name const&) = default;                            \
      __name& operator =(__name&&) = default;                                 \
                                                                              \
      ELLE_SERIALIZE_FRIEND_FOR(__name<T>);                                   \
    };                                                                        \
                                                                              \
    template <typename T>                                                     \
    __name<T>                                                                 \
    __lower_name(T& value)                                                    \
    {                                                                         \
      return __name<T>(value);                                                \
    }                                                                         \
                                                                              \
  }                                                                           \
}                                                                             \
/**/

#endif
