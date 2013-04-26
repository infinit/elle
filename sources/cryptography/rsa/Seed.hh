#if defined(ELLE_CRYPTOGRAPHY_ROTATION)

# ifndef INFINIT_CRYPTOGRAPHY_RSA_SEED_HH
#  define INFINIT_CRYPTOGRAPHY_RSA_SEED_HH

#  include <cryptography/fwd.hh>
#  include <cryptography/Seed.hh>

#  include <elle/types.hh>
#  include <elle/attribute.hh>
#  include <elle/operator.hh>
#  include <elle/Buffer.hh>
#  include <elle/Printable.hh>
#  include <elle/serialize/fwd.hh>
#  include <elle/serialize/construct.hh>

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
      /// RSA key pairs, private or public keys given a buffer of random data
      /// and the modulus of the key to generate.
      class Seed:
        public cryptography::seed::Interface,
        public elle::serialize::SerializableMixin<Seed>
      {
        /*-------------.
        | Construction |
        `-------------*/
      public:
        Seed(); // XXX[to deserialize]
        /// Construct an RSA seed based on a buffer and the modulus which is
        /// especially used for deriving public keys.
        ///
        /// Note that this version of the constructor duplicates both the buffer
        /// and the modulus.
        explicit
        Seed(elle::Buffer const& buffer,
             ::BIGNUM* n);
        /// Construct an RSA seed as above but by transferring the ownership
        /// of both the buffer and the modulus.
        explicit
        Seed(elle::Buffer&& buffer,
             ::BIGNUM* n);
        Seed(Seed const& seed);
        Seed(Seed&& other);
        ELLE_SERIALIZE_CONSTRUCT_DECLARE(Seed);
        ~Seed();

        /*----------.
        | Operators |
        `----------*/
      public:
        elle::Boolean
        operator ==(Seed const& other) const;
        ELLE_OPERATOR_NEQ(Seed);
        ELLE_OPERATOR_NO_ASSIGNMENT(Seed);

        /*-----------.
        | Interfaces |
        `-----------*/
      public:
        // seed
        virtual
        elle::Boolean
        operator ==(cryptography::seed::Interface const& other) const;
        virtual
        cryptography::seed::Interface*
        clone() const;
        virtual
        Cryptosystem
        cryptosystem() const;
        // printable
        virtual
        void
        print(std::ostream& stream) const;
        // serializable
        ELLE_SERIALIZE_FRIEND_FOR(Seed);

        /*-----------.
        | Attributes |
        `-----------*/
      private:
        ELLE_ATTRIBUTE_R(elle::Buffer, buffer);
        ELLE_ATTRIBUTE_R(BIGNUM*, n);
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

        /// Generate a seed based on both the public and private RSA keys
        /// which will be used to rotate it.
        Seed
        generate(cryptography::publickey::Interface const& K,
                 cryptography::privatekey::Interface const& k);
      }
    }
  }
}

#  include <cryptography/rsa/Seed.hxx>

# endif

#endif
