#pragma once

namespace elle
{
  template <typename T>
  T&
  unconst(T const& o)
  {
    return const_cast<T&>(o);
  }

  template <typename T>
  T*
  unconst(T const* o)
  {
    return const_cast<T*>(o);
  }
}
