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
        -> decltype(elle::serialization::serialize<T, Json>
                    (o, std::forward<Args>(args)...))
      {
        return elle::serialization::serialize<T, Json>
          (o, std::forward<Args>(args)...);
      }
    }
  }
}

#endif
