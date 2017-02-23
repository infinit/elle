#ifndef ELLE_SERIALIZATION_BINARY_HH
# define ELLE_SERIALIZATION_BINARY_HH

# include <elle/serialization/binary/SerializerIn.hh>
# include <elle/serialization/binary/SerializerOut.hh>

namespace elle
{
  namespace serialization
  {
    class Binary
    {
    public:
      using SerializerIn = binary::SerializerIn;
      using SerializerOut = binary::SerializerOut;
    };

    namespace binary
    {
      template <typename T, typename ... Args>
      auto
      deserialize(Args&& ... args) -> auto
      {
        return elle::serialization::deserialize<Binary, T>
          (std::forward<Args>(args)...);
      }

      template <typename Serializer = void, typename T, typename ... Args>
      auto
      serialize(T const& o, Args&& ... args) -> auto
      {
        return elle::serialization::serialize<Binary, Serializer, T>
          (o, std::forward<Args>(args)...);
      }

      template <typename Serializer, typename T, typename ... Args>
      auto
      deserialize(Args&& ... args) -> auto
      {
        return elle::serialization::deserialize<Binary, Serializer, T>
          (std::forward<Args>(args)...);
      }
}
  }
}

#endif
