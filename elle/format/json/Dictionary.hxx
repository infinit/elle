#ifndef ELLE_FORMAT_JSON_DICT_HXX
# define ELLE_FORMAT_JSON_DICT_HXX

# include "Dictionary.hh"

# include "Object.hxx"

namespace elle { namespace format { namespace json {

      struct Dictionary::_DictProxy
        {
        private:
          Dictionary::MapType&  _map;
          std::string           _key;
          Object*               _value;

        public:
          _DictProxy(Dictionary::MapType& map, std::string&& key)
            : _map(map)
            , _key(key)
            , _value(nullptr)
          {
            auto it = _map.find(_key);
            if (it != _map.end())
              _value = it->second;
          }

          _DictProxy(Dictionary::MapType& map, std::string const& key)
            : _map(map)
            , _key(key)
            , _value(nullptr)
          {
            auto it = _map.find(_key);
            if (it != _map.end())
              _value = it->second;
          }

          _DictProxy(_DictProxy&& proxy)
            : _map(proxy._map)
            , _key(std::move(proxy._key))
            , _value(proxy._value)
          {}


        private:
          _DictProxy& operator =(_DictProxy const&);
          _DictProxy(_DictProxy const& proxy);

        public:
          template<typename T> inline typename std::enable_if<
              !std::is_base_of<Object, T>::value
            , _DictProxy&
          >::type operator =(T const& val)
              {
                typedef typename detail::SelectJSONType<T>::type JSONType;
                delete _value;
                _value = nullptr;
                _value = new JSONType(val);
                _map[_key] = _value;
                return *this;
              }

          template<typename T> inline typename std::enable_if<
              std::is_base_of<Object, T>::value
            , _DictProxy&
          >::type operator =(T const& val)
              {
                typedef typename detail::SelectJSONType<T>::type JSONType;
                delete _value;
                _value = nullptr;
                auto clone = val.Clone();
                _value = _map[_key] = clone.get();
                assert(_value != nullptr);
                clone.release(); // exception could happen in map insertion
                return *this;
              }

          template<typename T> inline typename std::enable_if<
              std::is_same<T, std::unique_ptr<Object>>::value
              , _DictProxy&
          >::type operator =(T&& val)
            {
              delete _value;
              _value = nullptr;
              _value = _map[_key] = val.get();
              assert(_value != nullptr);
              val.release(); // exception could happen in map insertion
              return *this;
            }
          template<typename T>
            bool operator ==(T const& val) const
            {
              if (_value == nullptr)
                throw Dictionary::KeyError(_key);
              return *_value == val;
            }

          template<typename T>
            inline bool operator !=(T const& val) const
              { return !(*this == val); }

          operator bool() const
            {
              return *this == true;
            }

          template<typename T>
            inline void Load(T& val) const
            {
              if (_value == nullptr)
                throw Dictionary::KeyError(_key);
              return _value->Load(val);
            }


          operator Object const&() const
            {
              if (_value == nullptr)
                throw Dictionary::KeyError(_key);
              return *_value;
            }
          std::string repr() const
            {
              if (_value == nullptr)
                throw Dictionary::KeyError(_key);
              return _value->repr();
            }
        };

      template<typename Container>
      Dictionary::Dictionary(Container const& container)
      {
        static_assert(
            detail::IsMap<Container>::value,
            "This container is not a map"
        );
        static_assert(
            detail::IsStringMap<Container>::value,
            "This map is not indexed with std::string"
        );
        auto it = container.begin(), end = container.end();
        for (; it != end; ++it)
          {
            _DictProxy(_map, it->first) = it->second;
          }
      }

      Dictionary::_DictProxy Dictionary::operator [](std::string const& key)
        { return _DictProxy(_map, key); }

}}} // !namespace elle::format::json

#endif
