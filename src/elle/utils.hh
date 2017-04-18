#pragma once

#include <type_traits>

namespace elle
{
  /// Unconst the given reference.
  template <typename T>
  T&
  unconst(T const& o)
  {
    return const_cast<T&>(o);
  }

  /// Unconst the given pointer.
  template <typename T>
  T*
  unconst(T const* o)
  {
    return const_cast<T*>(o);
  }

  /// Add const.
  /// See std::as_const in C++17.
  template <typename T>
  constexpr
  std::add_const_t<T>&
  as_const(T& t) noexcept
  {
    return t;
  }
}
