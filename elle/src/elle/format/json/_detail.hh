#ifndef ELLE_FORMAT_JSON_DETAIL_HH
# define ELLE_FORMAT_JSON_DETAIL_HH

# include <elle/format/json/Object.hh>
# include <elle/format/json/types.hh>

namespace elle
{
  namespace format
  {
    namespace json
    {
      namespace detail
      {

        template <bool Cond, typename Then, typename Else>
        struct StaticIf;

        template <typename Then, typename Else>
        struct StaticIf<true, Then, Else>
        {
          typedef Then type;
        };

        template <typename Then, typename Else>
        struct StaticIf<false, Then, Else>
        {
          typedef Else type;
        };

        template <typename T>
        struct FastConst
        {
          typedef typename StaticIf<
              std::is_arithmetic<T>::value ||
              (std::is_trivial<T>::value && (sizeof(T) <= 4 * sizeof(size_t)))
            , T
            , T const&
          >::type type;
        };

        template <typename T>
        class BasicObject
          : public elle::format::json::Object
        {
        private:
          typedef typename FastConst<T>::type FastConstType;
        public:
          typedef T Type;
          typedef FastConstType CastType;

        private:
          Type _value;

        public:
          BasicObject(FastConstType value = T{})
            : _value(value)
          {}

          BasicObject(BasicObject const& other)
            : _value(other._value)
          {}

          BasicObject(BasicObject&& other)
            : _value(other._value)
          {}

          BasicObject& operator =(BasicObject const& other)
          {
            _value = other._value;
            return *this;
          }

          BasicObject& operator =(BasicObject&& other)
          {
            _value = other._value;
            return *this;
          }

          BasicObject& operator =(FastConstType value)
          {
            _value = value;
            return *this;
          }

          Type const&
          value() const
          {
            return _value;
          }

          operator CastType() const { return _value; }

          using Object::operator ==;

          bool operator ==(FastConstType value) const
          {
            return _value == value;
          }

          virtual bool operator ==(Object const& other) const
          {
            return other == *this;
          }

          virtual bool operator ==(BasicObject const& other) const
          {
            return _value == other._value;
          }

          std::unique_ptr<Object> clone() const
          {
            return std::unique_ptr<Object>(new BasicObject(_value));
          }

          using Object::repr;
          virtual void repr(std::ostream& out) const;
        };

        template <typename T, typename KeyType = void>
        struct IsMappedWith
        {
          typedef char Yes;
          typedef struct { Yes _[2]; } No;
          static No f(...);
          template <typename K> static Yes f(K*, typename K::key_type*);
          static bool const value = (
                IsMap<T>::value
            &&  sizeof(
                  f(static_cast<T*>(nullptr),
                    static_cast<KeyType*>(nullptr))
                ) == sizeof(Yes)
          );
        };

        template <typename Container>
        struct IsStringMap
        {
          static bool const value = IsMappedWith<Container, std::string>::value;
        };

        template<typename T>
        struct SelectJSONType
        {
          typedef typename StaticIf<
            IsString<T>::value
          , String
          , typename StaticIf<
              std::is_same<T, bool>::value
            , Bool
            , typename StaticIf<
                std::is_floating_point<T>::value
              , Float
              , typename StaticIf<
                  std::is_integral<T>::value || std::is_enum<T>::value
                , Integer
                , typename StaticIf<
                    IsArray<T>::value
                  , Array
                  , typename StaticIf<
                      IsStringMap<T>::value
                    , Dictionary
                    , typename StaticIf<
                        std::is_base_of<Object, T>::value && !std::is_same<T, Object>::value
                      , T
                      , void
                      >::type
                    >::type
                  >::type
                >::type
              >::type
            >::type
          >::type type;
        };

      }
    }
  }
} // !namespace elle::format::json::detail

#endif /* ! _DETAIL_HH */
