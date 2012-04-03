#ifndef ELLE_FORMAT_JSON_DICT_HH
# define ELLE_FORMAT_JSON_DICT_HH

# include <unordered_map>

# include "Object.hh"
# include "Bool.hh"
# include "Null.hh"

# include "_detail.hh"

namespace elle { namespace format { namespace json {

    struct Dict : public Object
    {
    private:
      typedef std::unordered_map<std::string, Object*> MapType;

      /// Wrapper to set item into the map
      struct _DictProxy
        {
        private:
          MapType&            _map;
          std::string         _key;
          Object*             _value;

        public:
          _DictProxy(MapType& map, std::string&& key)
            : _map(map)
            , _key(key)
            , _value(_map[_key])
          {}

          _DictProxy(MapType& map, std::string const& key)
            : _map(map)
            , _key(key)
            , _value(_map[_key])
          {}

          _DictProxy(_DictProxy&& proxy)
            : _map(proxy._map)
            , _key(std::move(proxy._key))
            , _value(proxy._value)
          {}


        private:
          _DictProxy& operator =(_DictProxy const&);
          _DictProxy(_DictProxy const& proxy);

        public:
          _DictProxy& operator =(bool val)
            {
              delete _value;
              _map[_key] = _value = new Bool(val);
              return *this;
            }

          template<typename T> inline typename std::enable_if<
              std::is_integral<T>::value
              , _DictProxy&
          >::type operator =(T val);

          template<typename T> inline typename std::enable_if<
              std::is_floating_point<T>::value
              , _DictProxy&
          >::type operator =(T val);

          template<typename T> inline typename std::enable_if<
              detail::IsString<T>::value
              , _DictProxy&
          >::type operator =(T const& val);

          template<typename T> inline typename std::enable_if<
              detail::IsArray<T>::value
              , _DictProxy&
          >::type operator =(T const& val);

          template<typename T> inline typename std::enable_if<
              detail::IsStringMap<T>::value
              , _DictProxy&
          >::type operator =(T const& val);

          template<typename T> inline typename std::enable_if<
              std::is_base_of<T, Object>::value
              , _DictProxy&
          >::type operator =(Object const& val);

          template<typename T> inline typename std::enable_if<
              std::is_same<T, std::unique_ptr<Object>>::value
              , _DictProxy&
          >::type operator =(T&& val);
        };

      friend class OutputJSONArchive;
      friend struct _DictProxy;

    private:
      MapType _map;

    public:
      Dict() {}

      template<typename Container>
      Dict(Container const& container,
           typename std::enable_if<detail::IsStringMap<Container>::value, bool>::type = true)
      {
        auto it = container.begin(), end = container.end();
        for (; it != end; ++it)
          {
            _DictProxy(_map, it->first) = it->second;
          }
      }

      ~Dict()
      {
        for (auto it = _map.begin(), end = _map.end(); it != end; ++it)
          delete it->second;
        _map.clear();
      }

      _DictProxy operator [](std::string&& key)
        { return _DictProxy(_map, key); }

      _DictProxy operator [](std::string const& key)
        { return _DictProxy(_map, key); }

      Object const& operator [](std::string const& key) const
      {
        auto it = _map.find(key);
        if (it == _map.end())
          return null;
        return *(it->second);
      }

      size_t size() const { return _map.size(); }

    protected:
      void Save(OutputJSONArchive& ar) const;
      std::unique_ptr<Object> Clone() const;
    };

}}} // !namespace elle::format::json

#endif /* ! DICT_HH */


