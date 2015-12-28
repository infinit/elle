#ifndef ELLE_LAZY_HXX
# define ELLE_LAZY_HXX

namespace elle
{
  template <typename T>
  Lazy<T>::Lazy(Lambda const& lambda):
    _lambda(lambda),
    _value()
  {}

  template <typename T>
  typename Lazy<T>::Value&
  Lazy<T>::value()
  {
    if (!this->_value)
      this->_value.emplace(this->_lambda());
    return *this->_value;
  }

  template <typename T>
  typename Lazy<T>::Value const&
  Lazy<T>::value() const
  {
    return const_cast<Self*>(this)->value();
  }

  template <typename T>
  typename Lazy<T>::Value*
  Lazy<T>::operator -> ()
  {
    return &value();
  }

  template <typename T>
  typename Lazy<T>::Value const*
  Lazy<T>::operator -> () const
  {
    return &value();
  }

  template <typename T>
  std::ostream&
  operator << (std::ostream& output, Lazy<T> const& lazy)
  {
    output << lazy.value();
    return output;
  }

  template <typename F>
  auto
  lazy(F&& f) -> Lazy<decltype(f())>
  {
    return Lazy<decltype(f())>(std::forward<F>(f));
  }
}

#endif
