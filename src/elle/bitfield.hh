#pragma once

#include <type_traits>

namespace elle
{
  namespace _details
  {
    template <typename T>
    struct MakeBitfield
    {
      static constexpr bool value = false;
    };
  }
}

#define ELLE_BITFIELD(T)                        \
  namespace elle                                \
  {                                             \
    namespace _details                          \
    {                                           \
      template <>                               \
      struct MakeBitfield<T>                    \
      {                                         \
        static const bool value = true;         \
      };                                        \
    }                                           \
  }                                             \


#define ELLE_BITFIELD_BINARY_OP(Op)                                     \
  template<typename T>                                                  \
  typename std::enable_if<elle::_details::MakeBitfield<T>::value, T>::type \
  operator Op(T lhs, T rhs)                                             \
  {                                                                     \
    static_assert(std::is_enum<T>::value,                               \
                  "template parameter is not an enum type");            \
    using underlying = typename std::underlying_type<T>::type;          \
    return static_cast<T>(                                              \
      static_cast<underlying>(lhs) Op static_cast<underlying>(rhs)      \
      );                                                                \
  }

ELLE_BITFIELD_BINARY_OP(|)
ELLE_BITFIELD_BINARY_OP(&)

#undef ELLE_BITFIELD_BINARY_OP

#define ELLE_BITFIELD_UNARY_OP(R, Op)                                   \
  template<typename T>                                                  \
  typename std::enable_if<elle::_details::MakeBitfield<T>::value, R>::type \
  operator Op(T b)                                                      \
  {                                                                     \
    static_assert(std::is_enum<T>::value,                               \
                  "template parameter is not an enum type");            \
    using underlying = typename std::underlying_type<T>::type;          \
    return R(Op(static_cast<underlying>(b)));                           \
  }                                                                     \

ELLE_BITFIELD_UNARY_OP(bool, !)
ELLE_BITFIELD_UNARY_OP(T, ~)

#undef ELLE_BITFIELD_UNARY_OP
