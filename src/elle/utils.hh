#ifndef ELLE_UTILS_HH
# define ELLE_UTILS_HH

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

#endif
