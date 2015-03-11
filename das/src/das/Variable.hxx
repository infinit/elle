#ifndef DAS_VARIABLE_HXX
# define DAS_VARIABLE_HXX

namespace das
{
  template <typename T>
  Variable<T>&
  Variable<T>::operator =(T const& value)
  {
    this->_value = value;
    return *this;
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
