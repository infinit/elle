#ifndef ELLE_CRYPTOGRAPHY_SERIALIZATION_HXX
# define ELLE_CRYPTOGRAPHY_SERIALIZATION_HXX

namespace elle
{
  namespace cryptography
  {
    /*----------.
    | Functions |
    `----------*/

    template <typename S>
    void
    serialize(elle::serialization::Serializer& serializer,
              typename S::Type*& data)
    {
      if (serializer.out())
      {
        ELLE_ASSERT_NEQ(data, nullptr);
        elle::Buffer representation = S::encode(data);
        serializer.serialize(S::identifier, representation);
      }
      else
      {
        elle::Buffer representation;
        serializer.serialize(S::identifier, representation);
        data = S::decode(representation);
      }
    }
  }
}

#endif
