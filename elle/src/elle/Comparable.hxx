#ifndef ELLE_COMPARABLE_HXX
# define ELLE_COMPARABLE_HXX

namespace elle
{
  template <typename Self, typename T>
  inline
  bool
  Comparable<Self, T>::operator == (T const& other) const
  {
    return static_cast<Self const*>(this)->_compare(other);
  }

  template <typename Self, typename T>
  inline
  bool
  Comparable<Self, T>::operator != (T const& other) const
  {
    return !static_cast<Self const*>(this)->_compare(other);
  }
}

#endif
