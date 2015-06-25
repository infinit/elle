#include <cryptography/rsa/serialization.hh>
#include <cryptography/rsa/der.hh>

#include <elle/assert.hh>
#include <elle/Buffer.hh>

#include <openssl/rsa.h>

namespace infinit
{
  namespace cryptography
  {
    namespace rsa
    {
      /*----------.
      | Functions |
      `----------*/

      void
      serialize_public(elle::serialization::Serializer& serializer,
                       ::RSA*& rsa)
      {
        if (serializer.out())
        {
          ELLE_ASSERT_NEQ(rsa, nullptr);
          elle::Buffer representation =
            infinit::cryptography::rsa::der::encode_public(rsa);
          serializer.serialize("rsa", representation);
        }
        else
        {
          elle::Buffer representation;
          serializer.serialize("rsa", representation);
          ELLE_ASSERT_EQ(rsa, nullptr);
          rsa = infinit::cryptography::rsa::der::decode_public(representation);
        }
      }

      void
      serialize_private(elle::serialization::Serializer& serializer,
                        ::RSA*& rsa)
      {
        if (serializer.out())
        {
          ELLE_ASSERT_NEQ(rsa, nullptr);
          elle::Buffer representation =
            infinit::cryptography::rsa::der::encode_private(rsa);
          serializer.serialize("rsa", representation);
        }
        else
        {
          elle::Buffer representation;
          serializer.serialize("rsa", representation);
          ELLE_ASSERT_EQ(rsa, nullptr);
          rsa = infinit::cryptography::rsa::der::decode_private(representation);
        }
      }
    }
  }
}
