#ifndef ELLE_FORMAT_JSON_DETAIL_HH
# define ELLE_FORMAT_JSON_DETAIL_HH

namespace elle { namespace format { namespace json { namespace detail {

    template<typename T> struct FastConst
      {
        typedef T const& type;
      };
# define fast_is_value(T) template<> struct FastConst<T> { typedef T type; }
    fast_is_value(int32_t);
    fast_is_value(double);
    fast_is_value(bool);
# undef fast_is_value

    template<typename T>
      class BasicObject
        : public elle::format::json::Object
      {
      private:
        typedef typename FastConst<T>::type FastConstType;
      public:
        typedef T Type;

      private:
        Type _value;

      public:
        BasicObject(FastConstType value = T{}) : _value(value) {}
        BasicObject(BasicObject const& other) : _value(other._value) {}
        BasicObject(BasicObject&& other) : _value(other._value) {}
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
        operator FastConstType() const { return _value; }
        bool operator ==(FastConstType value) const
        {
          return _value == value;
        }

        std::unique_ptr<Object> Clone() const
          { return std::unique_ptr<Object>(new BasicObject(_value)); }

      protected:
        void Save(elle::serialize::OutputJSONArchive& ar) const;
      };

    template<typename T>
      struct IsString {
          static bool const value = false;
      };
    template<>
      struct IsString<std::string> {
          static bool const value = true;
      };
    template<>
      struct IsString<std::wstring> {
          static bool const value = true;
      };


    template<typename T>
      struct HasIterator
        {
          typedef char Yes;
          typedef struct { Yes _[2]; } No;
          static No f(...);
          template<typename K>
            static Yes f(K*, typename K::iterator* = nullptr);
          static bool const value =
            sizeof(f(static_cast<T*>(nullptr))) == sizeof(Yes);
        };

    template<typename T, typename KeyType = void>
      struct IsMappedWith
        {
          typedef char Yes;
          typedef struct { Yes _[2]; } No;
          static No f(...);
          template<typename K>
            static Yes f(K*, typename K::key_type*);
          static bool const value =
            sizeof(f(static_cast<T*>(nullptr), static_cast<KeyType*>(nullptr))) == sizeof(Yes);
        };

    template<typename Container>
      struct IsStringMap {
          static bool const value = IsMappedWith<Container, std::string>::value;
      };

    template<typename T>
      struct IsArray {
          static bool const value = HasIterator<T>::value && !IsString<T>::value;
      };

}}}} // !namespace elle::format::json::detail

#endif /* ! _DETAIL_HH */


