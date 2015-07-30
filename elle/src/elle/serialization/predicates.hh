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
        !std::is_base_of<boost::optional_detail::optional_tag, T>::value &&
        std::is_constructible<T, SerializerIn&>::value,
        bool>::type
      _is_unserializable_inplace(int)
      {
        return true;
      };

      template <typename T>
      constexpr
      bool
      _is_unserializable_inplace(unsigned int)
      {
        return false;
      };

      template <typename T>
      constexpr
      bool
      is_unserializable_inplace()
      {
        return _is_unserializable_inplace<T>(42);
      };
    }
  }
}

#endif
