#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)

# ifndef INFINIT_CRYPTOGRAPHY_RSA_SEED_HH
#  define INFINIT_CRYPTOGRAPHY_RSA_SEED_HH

#  include <cryptography/fwd.hh>
#  include <cryptography/types.hh>

#  include <elle/types.hh>
#  include <elle/attribute.hh>
#  include <elle/operator.hh>
#  include <elle/Buffer.hh>
#  include <elle/Printable.hh>
#  include <elle/serialize/fwd.hh>
#  include <elle/serialize/construct.hh>

#  include <utility>
ELLE_OPERATOR_RELATIONALS();

//
// ---------- Class -----------------------------------------------------------
//

namespace infinit
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
        Seed(); // XXX[to deserialize]
        /// Construct an RSA seed based on a buffer and the length of the
        /// keys' modulus it will allow generating.
        ///
        /// Note that this version of the constructor duplicates the buffer.
        explicit
        Seed(elle::Buffer const& buffer,
             elle::Natural32 const length);
        /// Construct an RSA seed as above but by transferring the ownership
        /// of the buffer.
        explicit
        Seed(elle::Buffer&& buffer,
             elle::Natural32 const length);
        Seed(Seed const& seed);
        Seed(Seed&& other);
        ELLE_SERIALIZE_CONSTRUCT_DECLARE(Seed);
        virtual
        ~Seed() = default;

        /*----------.
        | Operators |
        `----------*/
      public:
        elle::Boolean
        operator ==(Seed const& other) const;
        ELLE_OPERATOR_NO_ASSIGNMENT(Seed);

        /*-----------.
        | Interfaces |
        `-----------*/
      public:
        // printable
        void
        print(std::ostream& stream) const override;
        // serializable
        ELLE_SERIALIZE_FRIEND_FOR(Seed);

        /*-----------.
        | Attributes |
        `-----------*/
      private:
        ELLE_ATTRIBUTE_R(elle::Buffer, buffer);
        // The length, in bits, of the keys' modulus one can
        // generate with this seed.
        ELLE_ATTRIBUTE_R(elle::Natural32, length);
      };
    }
  }
}

//
// ---------- Generator -------------------------------------------------------
//

namespace infinit
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
        generate(elle::Natural32 const length);
        /// Generate a seed for the given key pair.
        Seed
        generate(KeyPair const& keypair);
      }
    }
  }
}

#  include <cryptography/rsa/Seed.hxx>

# endif

#endif
