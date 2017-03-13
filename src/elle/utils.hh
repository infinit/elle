#pragma once

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
}
