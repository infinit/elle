#ifndef ELLE_FORMAT_JSON_ARRAY_HXX
# define ELLE_FORMAT_JSON_ARRAY_HXX

# include "Bool.hh"
# include "Dictionary.hh"
# include "Float.hh"
# include "Integer.hh"
# include "Null.hh"
# include "String.hh"

using namespace elle::format::json;

template<typename T> inline typename std::enable_if<
    std::is_integral<T>::value
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
>::type Array::push_back(T const& value)
  { _value.push_back(static_cast<Object const&>(value).Clone().release()); }

#endif
