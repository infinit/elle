#ifndef ELLE_FORMAT_JSON_ARRAY_HXX
# define ELLE_FORMAT_JSON_ARRAY_HXX

# include <boost/foreach.hpp>

# include "Bool.hh"
# include "Dictionary.hh"
# include "Float.hh"
# include "Integer.hh"
# include "Null.hh"
# include "String.hh"
# include "Array.hh"

using namespace elle::format::json;

template<typename Container>
Array::Array(Container const& container,
            typename std::enable_if<detail::IsArray<Container>::value, bool>::type)
{
  BOOST_FOREACH(auto val, container)
  {
    this->push_back(val);
  }
}

template<typename T> inline typename std::enable_if<
    std::is_same<T, Null>::value
>::type Array::push_back(T const&)
  { _value.push_back(new Null); }

template<typename T> inline typename std::enable_if<
    std::is_same<T , bool>::value
>::type Array::push_back(T value)
  { _value.push_back(new Bool(value)); }

template<typename T> inline typename std::enable_if<
        std::is_integral<T>::value
    &&  !std::is_same<T , bool>::value
>::type Array::push_back(T value)
  { _value.push_back(new Integer(value)); }

template<typename T> inline typename std::enable_if<
    std::is_floating_point<T>::value
>::type Array::push_back(T value)
  { _value.push_back(new Float(value)); }

template<typename T> inline typename std::enable_if<
    detail::IsString<T>::value
>::type Array::push_back(T const& value)
  { _value.push_back(new String(value)); }

template<typename T> inline typename std::enable_if<
    detail::IsArray<T>::value
>::type Array::push_back(T const& value)
  { _value.push_back(new Array(value)); }

template<typename T> inline typename std::enable_if<
    detail::IsStringMap<T>::value
>::type Array::push_back(T const& value)
  { _value.push_back(new Dictionary(value)); }

template<typename T> inline typename std::enable_if<
      std::is_base_of<Object, T>::value
  &&  !std::is_same<T, Null>::value
>::type Array::push_back(T const& value)
  { _value.push_back(static_cast<Object const&>(value).Clone().release()); }

template<typename T> inline typename std::enable_if<
    std::is_pointer<T>::value && !std::is_array<T>::value
>::type Array::push_back(T const& value)
  { static_assert(!std::is_pointer<T>::value, "You cannot insert pointers in an array"); }
#endif
