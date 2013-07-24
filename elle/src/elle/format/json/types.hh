#ifndef ELLE_FORMAT_JSON_DETAIL_TYPES_HH
# define ELLE_FORMAT_JSON_DETAIL_TYPES_HH

namespace elle
{
  namespace format
  {
    namespace json
    {
      namespace detail
      {
        template <typename T>
        struct HasIterator
        {
          typedef char Yes;
          typedef struct { Yes _[2]; } No;
          static No f(...);
          template <typename K>
          static Yes f(K*, typename K::iterator* = nullptr);
          static bool const value =
            sizeof(f(static_cast<T*>(nullptr))) == sizeof(Yes);
        };

        template <typename T>
        struct IsString
        {
          static bool const value = (
                std::is_same<T, std::string>::value
            ||  std::is_convertible<T, char const* const>::value
          );
        };

        template <typename T>
        struct IsMap
        {
          typedef char Yes;
          typedef struct { Yes _[2]; } No;
          static No f(...);
          template <typename K>
          static Yes f(K*, typename K::mapped_type* = nullptr,
                       typename K::key_type* = nullptr);
          static bool const value = (
                HasIterator<T>::value
            &&  sizeof(f(static_cast<T*>(nullptr))) == sizeof(Yes)
          );
        };

        template <typename T>
        struct IsArray
        {
          static bool const value = (
                std::is_array<T>::value
            ||  (
                    HasIterator<T>::value
                &&  !IsString<T>::value
                &&  !IsMap<T>::value
            )
          );
        };

        template <typename T>
        struct ObjectCanLoad
        {
          static bool const value = (
                std::is_arithmetic<T>::value
            || std::is_enum<T>::value
            ||  std::is_same<T, std::string>::value
            ||  std::is_same<T, bool>::value
            ||  detail::IsArray<T>::value
          );
        };
      }
    }
  }
}

#endif
