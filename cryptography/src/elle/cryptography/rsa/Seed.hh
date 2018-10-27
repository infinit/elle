#pragma once

#if defined ELLE_CRYPTOGRAPHY_ROTATION

#include <utility>

#include <elle/cryptography/fwd.hh>
#include <elle/attribute.hh>
#include <elle/operator.hh>
#include <elle/Buffer.hh>
#include <elle/Printable.hh>
#include <elle/serialization/Serializer.

//
// ---------- Class -----------------------------------------------------------
//

namespace elle
{
  namespace cryptography
  {
    namespace rsa
    {
      /// Represent an RSA seed which can be used to deterministically generate
      /// RSA key pairs, private or public keys given a buffer of random data.
      class Seed:
        public elle::Printable
      {
        /*-------------.
        | Construction |
        `-------------*/
      public:
        /// Construct an RSA seed based on a buffer and the length of the
        /// keys' modulus it will allow generating.
        ///
        /// Note that this version of the constructor duplicates the buffer.
        explicit
        Seed(elle::Buffer const& buffer,
             uint32_t const length);
        /// Construct an RSA seed as above but by transferring the ownership
        /// of the buffer.
        explicit
        Seed(elle::Buffer&& buffer,
             uint32_t const length);
        Seed(Seed const& seed);
        Seed(Seed&& other);
        virtual
        ~Seed() = default;

        /*----------.
        | Operators |
        `----------*/
      public:
        bool
        operator ==(Seed const& other) const;
        ELLE_OPERATOR_NO_ASSIGNMENT(Seed);

        /*----------.
        | Printable |
        `----------*/
      public:
        void
        print(std::ostream& stream) const override;

        /*--------------.
        | Serialization |
        `--------------*/
      public:
        Seed(elle::serialization::SerializerIn& serializer);
        void
        serialize(elle::serialization::Serializer& serializer);
        using serialization_tag = elle::serialization_tag;

        /*-----------.
        | Attributes |
        `-----------*/
      private:
        ELLE_ATTRIBUTE_R(elle::Buffer, buffer);
        // The length, in bits, of the keys' modulus one can
        // generate with this seed.
        ELLE_ATTRIBUTE_R(uint32_t, length);
      };
    }
  }
}

//
// ---------- Generator -------------------------------------------------------
//

namespace elle
{
  namespace cryptography
  {
    namespace rsa
    {
      namespace seed
      {
        /*----------.
        | Functions |
        `----------*/

        /// Generate a seed of the given length.
        Seed
        generate(uint32_t const length);
        /// Generate a seed for the given key pair.
        Seed
        generate(KeyPair const& keypair);
      }
    }
  }
}

#endif
