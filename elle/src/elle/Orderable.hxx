#ifndef ELLE_ORDERABLE_HXX
# define ELLE_ORDERABLE_HXX

namespace elle
{
  template <typename Self, typename T>
  inline
  bool
  Orderable<Self, T>::operator < (T const& other) const
  {
    return static_cast<Self const*>(this)->_order(other) == Order::less;
  }

  template <typename Self, typename T>
  inline
  bool
  Orderable<Self, T>::operator <= (T const& other) const
  {
    return static_cast<Self const*>(this)->_order(other) != Order::more;
  }

  template <typename Self, typename T>
  inline
  bool
  Orderable<Self, T>::operator > (T const& other) const
  {
    return static_cast<Self const*>(this)->_order(other) == Order::more;
  }

  template <typename Self, typename T>
  inline
  bool
  Orderable<Self, T>::operator >= (T const& other) const
  {
    return static_cast<Self const*>(this)->_order(other) != Order::less;
  }

  template <typename Self, typename T>
  inline
  bool
  Orderable<Self, T>::_compare(T const& other) const
  {
    return static_cast<Self const*>(this)->_order(other) == Order::equal;
  }
}

#endif
