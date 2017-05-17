#pragma once

#include <elle/serialization/json/SerializerIn.hh>
#include <elle/serialization/json/SerializerOut.hh>

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
      /// Deserialize an instance of T represented in JSON.
      ///
      /// @tparam T          The expected type.
      /// @tparam Serializer The type of Serializer in use.
      /// @tparam ...Args    The types of other arguments.
      /// @param ...args     The arguments.
      /// @returns A instance of T.
      template <typename T, typename Serializer = void, typename ... Args>
      auto
      deserialize(Args&& ... args) -> auto
      {
        return elle::serialization::deserialize<Json, T, Serializer>
          (std::forward<Args>(args)...);
      }

      /// Serialize an instance of T to JSON.
      ///
      /// @tparam Serializer  The type of Serializer in use.
      /// @tparam ...Args     The types of other arguments.
      /// @param o            The object to serialize.
      /// @param ...args      The arguments.
      /// @returns A
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
