#ifndef ELLE_SERIALIZATION_FWD_HH
# define ELLE_SERIALIZATION_FWD_HH

namespace elle
{
  namespace serialization
  {
    class Serializable;
    class Serializer;
    class SerializerIn;
    class SerializerOut;

    class VirtuallySerializable
    {
    public:
      virtual
      ~VirtuallySerializable()
      {}

      virtual
      void
      serialize(Serializer& s) = 0;

      static constexpr char const* virtually_serializable_key = ".type";
    };
  }
}

#endif
