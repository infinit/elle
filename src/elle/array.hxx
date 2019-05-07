#pragma once

#include <ostream>

#include <elle/print/object.hh>

namespace std
{
  template <typename T, std::size_t N>
  ostream&
  operator <<(ostream& out, array<T, N> const& s)
  {
    out << '[';
    auto* sep = "";
    for (auto const& e: s)
    {
      elle::_details::print(out, sep);
      elle::_details::print(out, e);
      sep = ", ";
    }
    out << ']';
    return out;
  }

#define ELLE_ARRAY_OP_BINARY(Op)                        \
  template <typename T, std::size_t N, typename S>      \
  auto                                                  \
  operator Op(std::array<T, N> const& v, S&& s)         \
    -> std::array<decltype(v[0] Op s), N>               \
  {                                                     \
    return std::apply(                                  \
      [&] (auto&& ... elts)                             \
      {                                                 \
        return std::array{elts Op s ...};               \
      },                                                \
      v);                                               \
  }                                                     \
                                                        \
  template <typename T, typename U, std::size_t N>      \
  auto                                                  \
  operator Op(std::array<T, N> const& lhs,              \
              std::array<U, N>  const& rhs)             \
  {                                                     \
    return std::apply(                                  \
      [&] (auto&& ... lhse)                             \
      {                                                 \
        return std::apply(                              \
          [&] (auto&& ... rhse)                         \
          {                                             \
            return std::array{(lhse Op rhse) ...};      \
          }, rhs);                                      \
      }, lhs);                                          \
  }                                                     \

#define ELLE_ARRAY_OP_UNARY(Op)                         \
  template <typename T, std::size_t N>                  \
  auto                                                  \
  operator Op(std::array<T, N> const& v)                \
    -> std::array<decltype(Op v[0]), N>                 \
  {                                                     \
    return std::apply(                                  \
      [&] (auto&& ... elts)                             \
      {                                                 \
        return std::array{Op elts ...};                 \
      },                                                \
      v);                                               \
  }                                                     \

  ELLE_ARRAY_OP_BINARY(+);
  ELLE_ARRAY_OP_BINARY(-);
  ELLE_ARRAY_OP_BINARY(*);
  ELLE_ARRAY_OP_BINARY(/);
  ELLE_ARRAY_OP_BINARY(%);
  ELLE_ARRAY_OP_BINARY(&&);
  ELLE_ARRAY_OP_BINARY(||);
  ELLE_ARRAY_OP_UNARY(+);
  ELLE_ARRAY_OP_UNARY(-);
  ELLE_ARRAY_OP_UNARY(!);
#undef ELLE_ARRAY_OP_BINARY
#undef ELLE_ARRAY_OP_UNARY

  /// Compare array with any tuple-like.
  template <typename T, std::size_t S, typename U>
  std::enable_if_t<std::tuple_size<U>::value == S, bool>
  operator ==(std::array<T, S> const& lhs, U const& rhs)
  {
    return std::apply(
      [&] (auto const& ... lhse)
      {
        return std::apply(
          [&] (auto const& ... rhse)
          {
            return ((lhse == rhse) && ... && true);
          }, rhs);
      } , lhs);
  }
}
