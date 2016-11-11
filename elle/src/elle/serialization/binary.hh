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
      typedef binary::SerializerIn SerializerIn;
      typedef binary::SerializerOut SerializerOut;
    };

    namespace binary
    {
      template <typename T, typename ... Args>
      auto
      serialize(T const& o, Args&& ... args)
        -> decltype(elle::serialization::serialize<Binary, T>
                    (o, std::forward<Args>(args)...))
      {
        return elle::serialization::serialize<Binary, T>
          (o, std::forward<Args>(args)...);
      }

      template <typename T, typename ... Args>
      auto
      deserialize(Args&& ... args)
        -> decltype(elle::serialization::deserialize<Binary, T>
                    (std::forward<Args>(args)...))
      {
        return elle::serialization::deserialize<Binary, T>
          (std::forward<Args>(args)...);
      }

      template <typename Serializer, typename T, typename ... Args>
      auto
      serialize(T const& o, Args&& ... args)
        -> decltype(elle::serialization::serialize<Binary, T>
                    (o, std::forward<Args>(args)...))
      {
        return elle::serialization::serialize<Serializer, Binary, T>
          (o, std::forward<Args>(args)...);
      }

      template <typename Serializer, typename T, typename ... Args>
      auto
      deserialize(Args&& ... args)
        -> decltype(elle::serialization::deserialize<Binary, T>
                    (std::forward<Args>(args)...))
      {
        return elle::serialization::deserialize<Serializer, Binary, T>
          (std::forward<Args>(args)...);
      }
}
  }
}

#endif
