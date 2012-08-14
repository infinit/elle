#ifndef ELLE_FORMAT_JSON_DICT_HXX
# define ELLE_FORMAT_JSON_DICT_HXX

# include "Factory.hh"
# include "Object.hxx"

namespace elle
{
  namespace format
  {
    namespace json
    {

      struct Dictionary::_DictProxy
      {
      private:
        internal::Dictionary& _map;
        std::string           _key;
        Object*               _value;

      public:
        _DictProxy(internal::Dictionary& map,
                   std::string&& key);
        _DictProxy(internal::Dictionary& map,
                   std::string const& key);
        _DictProxy(_DictProxy&& proxy);

      private:
        _DictProxy& operator =(_DictProxy const&);
        _DictProxy(_DictProxy const& proxy);

      public:
        template <typename T>
        inline typename
        std::enable_if<
            !std::is_base_of<Object, T>::value
          , _DictProxy&
        >::type
        operator =(T const& val)
        {
          delete _value;
          _value = nullptr;
          _value = Factory::construct(val).release();
          _map[_key] = _value;
          return *this;
        }

        template <typename T>
        inline typename
        std::enable_if<
            std::is_base_of<Object, T>::value
          , _DictProxy&
        >::type
        operator =(T const& val)
        {
          delete _value;
          _value = nullptr;
          _value = val.Clone().release();
          assert(_value != nullptr);
          _map[_key] = _value;
          return *this;
        }

        template <typename T>
        inline typename
        std::enable_if<
            std::is_same<T, std::unique_ptr<Object>>::value
            , _DictProxy&
        >::type
        operator =(T&& val)
        {
          delete _value;
          _value = val.release();
          assert(_value != nullptr);
          _map[_key] = _value;
          return *this;
        }

        template <typename T>
        inline
        bool
        operator ==(T const& val) const
        {
          if (_value == nullptr)
            throw Dictionary::KeyError(_key);
          return *_value == val;
        }

        template <typename T>
        inline
        bool
        operator !=(T const& val) const
        {
          return !(*this == val);
        }

        template <typename T>
        inline
        void
        Load(T& val) const
        {
          if (_value == nullptr)
            throw Dictionary::KeyError(_key);
          return _value->Load(val);
        }

        template <typename T>
        inline
        T
        as() const
        {
          if (_value == nullptr)
            throw Dictionary::KeyError(_key);
          return _value->as<T>();
        }

        Array&
        as_array();

        Bool&
        as_bool();

        Dictionary&
        as_dictionary();

        Float&
        as_float();

        Integer&
        as_integer();

        Null&
        as_null();

        String&
        as_string();

        operator Object const&() const;
        std::string repr() const;
      };

      template <typename Container>
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
        for (auto const& pair : container)
          {
            _DictProxy(_map, pair.first) = pair.second;
          }
      }

      size_t
      Dictionary::size() const
      {
        return _map.size();
      }

    }
  }
} // !namespace elle::format::json

#endif
