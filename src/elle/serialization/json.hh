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
      using SerializerIn = json::SerializerIn;
      using SerializerOut = json::SerializerOut;
    };

    namespace json
    {
      template <typename T, typename Serializer = void, typename ... Args>
      auto
      deserialize(Args&& ... args) -> auto
      {
        return elle::serialization::deserialize<Json, T, Serializer>
          (std::forward<Args>(args)...);
      }

      template <typename Serializer = void, typename T, typename ... Args>
      auto
      serialize(T const& o, Args&& ... args) -> auto
      {
        return elle::serialization::serialize<Json, Serializer, T>
          (o, std::forward<Args>(args)...);
      }
    }
  }
}

#endif
