namespace hole
{
  template <class C, typename T>
  inline bool
  Cmp<C, T>::operator<(const T & other) const
  {
    return static_cast<const C *> (this)->Cmp(other) == -1;
  }

  template <class C, typename T>
  inline bool
  Cmp<C, T>::operator<= (const T & other) const
  {
    return static_cast<const C *> (this)->Cmp(other) <= 0;
  }

  template <class C, typename T>
  inline bool
  Cmp<C, T>::operator==(const T & other) const
  {
    return static_cast<const C *> (this)->Cmp(other) == 0;
  }

  template <class C, typename T>
  inline bool
  Cmp<C, T>::operator>=(const T & other) const
  {
    return static_cast<const C *> (this)->Cmp(other) >= 0;
  }

  template <class C, typename T>
  inline bool
  Cmp<C, T>::operator>(const T & other) const
  {
    return static_cast<const C *> (this)->Cmp(other) > 0;
  }
}
