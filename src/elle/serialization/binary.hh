#pragma once

#include <elle/serialization/binary/SerializerIn.hh>
#include <elle/serialization/binary/SerializerOut.hh>

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
      /// Deserialize an instance of T represented in binary.
      ///
      /// @tparam T          The expected type.
      /// @tparam Serializer The type of Serializer in use.
      /// @tparam ...Args    The types of other arguments.
      /// @param ...args     The arguments.
      /// @returns           A instance of T.
      template <typename Serializer, typename T, typename ... Args>
      auto
      deserialize(Args&& ... args) -> auto
      {
        return elle::serialization::deserialize<Binary, Serializer, T>
          (std::forward<Args>(args)...);
      }

      template <typename T, typename ... Args>
      auto
      deserialize(Args&& ... args) -> auto
      {
        return elle::serialization::deserialize<Binary, T>
          (std::forward<Args>(args)...);
      }

      /// Serialize an instance of T to binary.
      ///
      /// @tparam         Serializer The type of Serializer in use.
      /// @tparam ...Args The types of other arguments.
      /// @param o        The object to serialize.
      /// @param ...args  The arguments.
      /// @returns A
      template <typename Serializer = void, typename T, typename ... Args>
      auto
      serialize(T const& o, Args&& ... args) -> auto
      {
        return elle::serialization::serialize<Binary, Serializer, T>
          (o, std::forward<Args>(args)...);
      }
    }
  }
}
