#ifndef ELLE_SERIALIZATION_FWD_HH
# define ELLE_SERIALIZATION_FWD_HH

# include <elle/fwd.hh>

namespace elle
{
  namespace serialization
  {
    class Serializable;
    class Serializer;
    class SerializerIn;
    class SerializerOut;

    class VirtuallySerializableBase
    {
    public:
      static constexpr char const* virtually_serializable_key = ".type";
    };

    template <typename H, bool Versioned>
    class VirtuallySerializable
      : public VirtuallySerializableBase
    {
    public:
      using Hierarchy = H;

      virtual
      ~VirtuallySerializable()
      {}

      virtual
      void
      serialize(Serializer& s) = 0;
    };

    template <typename H>
    class VirtuallySerializable<H, true>
      : public VirtuallySerializableBase
    {
    public:
      using Hierarchy = H;

      virtual
      ~VirtuallySerializable()
      {}

      virtual
      void
      serialize(Serializer& s, elle::Version const& version) = 0;
    };

    namespace _details
    {
      template <typename T, typename S>
      inline constexpr
      bool
      has_serialize_convert_api();
    }
  }
}

#endif
