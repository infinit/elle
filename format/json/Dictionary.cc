#include <ostream>

#include "elle/serialize/JSONArchive.hxx"

#include "Dictionary.hh"

namespace elle
{
  namespace format
  {
    namespace json
    {

      Dictionary::Dictionary():
        _map{}
      {}

      Dictionary::Dictionary(internal::Dictionary&& value):
        _map{value}
      {}

      Dictionary::~Dictionary()
      {
        for (auto& pair : _map)
          delete pair.second;
        _map.clear();
      }

      void
      Dictionary::Save(elle::serialize::OutputJSONArchive& ar) const
      {
        auto it = _map.begin(),
             end = _map.end();
        ar.stream() << '{';
        if (it != end)
          {
            ar << it->first;
            ar.stream() << ':';
            assert(it->second != nullptr);
            it->second->Save(ar);
            ++it;
            for (; it != end; ++it)
              {
                ar.stream() << ',';
                ar << it->first;
                ar.stream() << ':';
                it->second->Save(ar);
              }
          }
        ar.stream() << '}';
      }

      std::unique_ptr<Object>
      Dictionary::Clone() const
      {
        auto res = std::unique_ptr<Dictionary>(new Dictionary);
        for (auto it = _map.begin(), end = _map.end(); it != end; ++it)
          {
            assert(it->second != nullptr);
            res->_map[it->first] = it->second->Clone().release();
          }
        return std::unique_ptr<Object>(res.release());
      }

      void
      Dictionary::update(Dictionary const& other)
      {
        for (auto const& pair: other._map)
          {
            this->_map[pair.first] = pair.second->Clone().release();
          }
      }

      Dictionary::_DictProxy
      Dictionary::operator [](std::string const& key)
      {
        return _DictProxy(_map, key);
      }

      Object const&
      Dictionary::operator [](std::string const& key) const
      {
        auto it = _map.find(key);
        if (it == _map.end())
          throw KeyError(key);
        return *(it->second);
      }

      Object const*
      Dictionary::contains(std::string const& key) const
      {
        auto it = _map.find(key);
        if (it != _map.end())
          return it->second;
        return nullptr;
      }

      bool
      Dictionary::operator ==(Object const& other) const
      {
        return other == *this;
      }

      bool
      Dictionary::operator ==(Dictionary const& other) const
      {
        if (this->size() != other.size())
          return false;
        auto it = _map.begin(),
             end = _map.end();
        for (; it != end; ++it)
          {
            auto other_it = other._map.find(it->first);
            if (other_it == other._map.end() ||
                *other_it->second != *it->second)
              return false;
          }
        return true;
      }

      //- Dictionary::_DictProxy ----------------------------------------------

      Dictionary::_DictProxy::_DictProxy(internal::Dictionary& map,
                                         std::string&& key):
        _map(map),
        _key(key),
        _value(nullptr)
      {
        auto it = _map.find(_key);
        if (it != _map.end())
          _value = it->second;
      }

      Dictionary::_DictProxy::_DictProxy(internal::Dictionary& map,
                                         std::string const& key):
        _map(map),
        _key(key),
        _value(nullptr)
      {
        auto it = _map.find(_key);
        if (it != _map.end())
          _value = it->second;
      }

      Dictionary::_DictProxy::_DictProxy(_DictProxy&& proxy):
        _map(proxy._map),
        _key(std::move(proxy._key)),
        _value(proxy._value)
      {}

# define __JSON_DICTPROXY_AS(type)                                            \
      if (_value == nullptr) throw Dictionary::KeyError(_key);                \
      return dynamic_cast<type>(*_value)                                      \
      /**/

      Array&
      Dictionary::_DictProxy::as_array()
      {
        __JSON_DICTPROXY_AS(Array&);
      }

      Bool&
      Dictionary::_DictProxy::as_bool()
      {
        __JSON_DICTPROXY_AS(Bool&);
      }

      Dictionary&
      Dictionary::_DictProxy::as_dictionary()
      {
        __JSON_DICTPROXY_AS(Dictionary&);
      }

      Float&
      Dictionary::_DictProxy::as_float()
      {
        __JSON_DICTPROXY_AS(Float&);
      }

      Integer&
      Dictionary::_DictProxy::as_integer()
      {
        __JSON_DICTPROXY_AS(Integer&);
      }

      Null&
      Dictionary::_DictProxy::as_null()
      {
        __JSON_DICTPROXY_AS(Null&);
      }

      String&
      Dictionary::_DictProxy::as_string()
      {
        __JSON_DICTPROXY_AS(String&);
      }

# undef __JSON_DICTPROXY_AS

      Dictionary::_DictProxy::operator Object const&() const
      {
        if (_value == nullptr)
          throw Dictionary::KeyError(_key);
        return *_value;
      }

      std::string
      Dictionary::_DictProxy::repr() const
      {
        if (_value == nullptr)
          throw Dictionary::KeyError(_key);
        return _value->repr();
      }

    }
  }
} // !elle::format::json
