#ifndef ELLE_FORMAT_JSON_DICT_HXX
# define ELLE_FORMAT_JSON_DICT_HXX

# include "Bool.hh"
# include "Dict.hh"
# include "Float.hh"
# include "Int.hh"
# include "Null.hh"
# include "String.hh"

namespace elle { namespace format { namespace json {

    template<typename T> inline typename std::enable_if<
        std::is_integral<T>::value
        , Dict::_DictProxy&
    >::type Dict::_DictProxy::operator =(T val)
      {
        delete _value;
        _map[_key] = _value = new Int(val);
        return *this;
      }

    template<typename T> inline typename std::enable_if<
        std::is_floating_point<T>::value
        , Dict::_DictProxy&
    >::type Dict::_DictProxy::operator =(T val)
      {
        delete _value;
        _map[_key] = _value = new Float(val);
        return *this;
      }

    template<typename T> inline typename std::enable_if<
        detail::IsString<T>::value
        , Dict::_DictProxy&
    >::type Dict::_DictProxy::operator =(T const& val)
      {
        delete _value;
        _map[_key] = _value = new String(val);
        return *this;
      }

    template<typename T> inline typename std::enable_if<
        detail::IsArray<T>::value
        , Dict::_DictProxy&
    >::type Dict::_DictProxy::operator =(T const& val)
      {
        delete _value;
        _map[_key] = _value = new Array(val);
        return *this;
      }

    template<typename T> inline typename std::enable_if<
        detail::IsStringMap<T>::value
        , Dict::_DictProxy&
    >::type Dict::_DictProxy::operator =(T const& val)
      {
        delete _value;
        _map[_key] = _value = new Dict(val);
        return *this;
      }

    template<typename T> inline typename std::enable_if<
        std::is_base_of<T, Object>::value
        , Dict::_DictProxy&
    >::type Dict::_DictProxy::operator =(Object const& val)
      {
        delete _value;
        _map[_key] = _value = val.Clone().release();
        return *this;
      }

    template<typename T> inline typename std::enable_if<
        std::is_same<T, std::unique_ptr<Object>>::value
        , Dict::_DictProxy&
    >::type Dict::_DictProxy::operator =(T&& val)
      {
        delete _value;
        _map[_key] = _value = val.release();
        return *this;
      }

    //template<typename T> void Dict::_DictProxy::Load(T& val)
    //  {
    //    if (_value != nullptr)
    //      return _value->Load(val);
    //    else
    //      return json::null.Load(val);
    //  }

}}} // !namespace elle::format::json

#endif
