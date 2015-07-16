#ifndef ELLE_SERIALIZATION_PREDICATES_HH
# define ELLE_SERIALIZATION_PREDICATES_HH

namespace elle
{
  namespace serialization
  {
    namespace
    {
      template <typename T>
      constexpr
      typename std::enable_if<
        sizeof(T(ELLE_SFINAE_INSTANCE(SerializerIn))) >= 0, bool>::type
      _is_serializer_constructible(int)
      {
        return true;
      };

      template <typename T>
      constexpr
      bool
      _is_serializer_constructible(unsigned int)
      {
        return false;
      };

      template <typename T>
      constexpr
      bool
      is_serializer_constructible()
      {
        return _is_serializer_constructible<T>(42);
      };
    }
  }
}

#endif
