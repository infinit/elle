#ifndef DAS_VARIABLE_HXX
# define DAS_VARIABLE_HXX

# include <elle/serialization/Serializer.hh>

namespace das
{
  template <typename T>
  Variable<T>::Variable()
    : _value()
  {}

  template <typename T>
  Variable<T>::Variable(T const& value)
    : _value(value)
  {}

  template <typename T>
  Variable<T>::Variable(T&& value)
    : _value(std::move(value))
  {}

  template <typename T>
  template <typename U>
  Variable<T>::Variable(Variable<U> const& v)
    : Variable<T>(v.value())
  {}

  template <typename T>
  Variable<T>::Variable(Variable<T> const& v)
    : Variable<T>(v.value())
  {}

  template <typename T>
  template <typename U>
  Variable<T>::Variable(Variable<U>&& v)
    : _value(std::move(v._value))
    , _changed(std::move(v._changed))
  {}

  template <typename T>
  Variable<T>::Variable(Variable<T>&& v)
    : _value(std::move(v._value))
    , _changed(std::move(v._changed))
  {}

  template <typename T>
  Variable<T>&
  Variable<T>::operator =(T const& value)
  {
    this->_value = value;
    this->_changed(this->_value);
    return *this;
  }

  template <typename T>
  template <typename U>
  Variable<T>&
  Variable<T>::operator =(Variable<U> const& value)
  {
    return this->operator =(value.value());
  }

  template <typename T>
  Variable<T>&
  Variable<T>::operator =(Variable<T> const& value)
  {
    return this->operator =(value.value());
  }

  template <typename T>
  template <typename U>
  bool
  Variable<T>::operator ==(Variable<U> const& other) const
  {
    return this->value() == other.value();
  }

  template <typename T>
  Variable<T>::operator T const& () const
  {
    return this->_value;
  }

  template <typename T>
  std::ostream&
  operator << (std::ostream& o, Variable<T> const& v)
  {
    o << v.value();
    return o;
  }
}

namespace elle
{
  namespace serialization
  {
    template <typename T>
    struct Serialize<das::Variable<T>>
    {
      typedef T Type;
      static
      typename elle::_detail::attribute_r_type<Type>::type
      convert(das::Variable<T>& v)
      {
        return v.value();
      }

      static
      T
      convert(T& v)
      {
        return v;
      }
    };
  }
}

#endif
