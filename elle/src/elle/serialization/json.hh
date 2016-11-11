#ifndef ELLE_SERIALIZATION_JSON_HH
# define ELLE_SERIALIZATION_JSON_HH

# include <elle/serialization/json/SerializerIn.hh>
# include <elle/serialization/json/SerializerOut.hh>

namespace elle
{
  namespace serialization
  {
    class Json
    {
    public:
      typedef json::SerializerIn SerializerIn;
      typedef json::SerializerOut SerializerOut;
    };

    namespace json
    {
      template <typename T, typename ... Args>
      auto
      serialize(T const& o, Args&& ... args)
        -> decltype(elle::serialization::serialize<Json, T>
                    (o, std::forward<Args>(args)...))
      {
        return elle::serialization::serialize<Json, T>
          (o, std::forward<Args>(args)...);
      }

      template <typename T, typename Serializer = void, typename ... Args>
      auto
      deserialize(Args&& ... args)
        -> decltype(elle::serialization::deserialize<Json, T>
                    (std::forward<Args>(args)...))
      {
        return elle::serialization::deserialize<Json, T, Serializer>
          (std::forward<Args>(args)...);
      }

      template <typename Serializer, typename T, typename ... Args>
      auto
      serialize(T const& o, Args&& ... args)
        -> decltype(elle::serialization::serialize<Json, T>
                    (o, std::forward<Args>(args)...))
      {
        return elle::serialization::serialize<Serializer, Json, T>
          (o, std::forward<Args>(args)...);
      }
    }
  }
}

#endif
