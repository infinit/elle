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
  Variable<T>&
  Variable<T>::operator =(T const& value)
  {
    this->_value = value;
    return *this;
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
      Type const&
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
