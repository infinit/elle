#ifndef ELLE_FORMAT_JSON_DETAIL_HH
#define ELLE_FORMAT_JSON_DETAIL_HH

namespace elle { namespace format { namespace json { namespace detail {

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

}}} // !namespace elle::json::detail

#endif /* ! _DETAIL_HH */


