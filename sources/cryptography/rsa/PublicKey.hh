#ifndef INFINIT_CRYPTOGRAPHY_RSA_PUBLICKEY_HH
# define INFINIT_CRYPTOGRAPHY_RSA_PUBLICKEY_HH

# include <cryptography/fwd.hh>
# include <cryptography/Code.hh>
# include <cryptography/Clear.hh>
# include <cryptography/PublicKey.hh>

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>
# include <elle/serialize/fwd.hh>
# include <elle/serialize/construct.hh>
# include <elle/concept/Uniquable.hh>

# include <utility>
ELLE_OPERATOR_RELATIONALS();

# include <openssl/evp.h>

namespace infinit
{
  namespace cryptography
  {
    namespace rsa
    {
      /// Represent a public key in the RSA asymmetric cryptosystem.
      class PublicKey:
        public publickey::Interface,
        public elle::serialize::SerializableMixin<PublicKey>,
        public elle::concept::MakeUniquable<PublicKey>
      {
        /*-------------.
        | Construction |
        `-------------*/
      public:
        PublicKey(); // XXX[to deserialize]
        /// Construct a public key based on the given EVP_PKEY key whose
        /// ownership is transferred to the public key.
        explicit
        PublicKey(::EVP_PKEY* key);
        /// Construct a private key by transferring ownership of some big
        /// numbers so as to build an EVP_PKEY.
        PublicKey(::BIGNUM* n,
                  ::BIGNUM* e);
        PublicKey(PublicKey const& other);
        PublicKey(PublicKey&& other);
        ELLE_SERIALIZE_CONSTRUCT_DECLARE(PublicKey);
        ~PublicKey();

        /*--------.
        | Methods |
        `--------*/
      private:
        /// Construct the object based on big numbers.
        ///
        /// Note that when called, the number are already allocated for the
        /// purpose of the object construction. In other words, the ownership
        /// is transferred to the public key being constructed. Thus, it is
        /// the responsibility of the public key being constructed to release
        /// memory should an error occur, i.e not the caller's
        void
        _construct(::BIGNUM* n,
                   ::BIGNUM* e);
        /// Prepare the public key cryptographic contexts.
        void
        _prepare();

        /*----------.
        | Operators |
        `----------*/
      public:
        elle::Boolean
        operator ==(PublicKey const& other) const;
        elle::Boolean
        operator <(PublicKey const& other) const;
        ELLE_OPERATOR_NO_ASSIGNMENT(PublicKey);

        /*-----------.
        | Interfaces |
        `-----------*/
      public:
        // publickey
        virtual
        elle::Boolean
        operator ==(publickey::Interface const& other) const;
        virtual
        elle::Boolean
        operator <(publickey::Interface const& other) const;
        virtual
        publickey::Interface*
        clone() const;
        virtual
        elle::Natural32
        size() const;
        virtual
        elle::Natural32
        length() const;
        virtual
        Cryptosystem
        cryptosystem() const;
        virtual
        Code
        encrypt(Plain const& plain) const;
        virtual
        elle::Boolean
        verify(Signature const& signature,
               Plain const& plain) const;
        virtual
        Clear
        decrypt(Code const& code) const;
        // printable
        virtual
        void
        print(std::ostream& stream) const;
        // serializable
        ELLE_SERIALIZE_FRIEND_FOR(PublicKey);

        /*-----------.
        | Attributes |
        `-----------*/
      private:
        ELLE_ATTRIBUTE(::EVP_PKEY*, key);
        ELLE_ATTRIBUTE(::EVP_PKEY_CTX*, context_encrypt);
        ELLE_ATTRIBUTE(::EVP_PKEY_CTX*, context_verify);
        ELLE_ATTRIBUTE(::EVP_PKEY_CTX*, context_decrypt);
      };
    }
  }
}

# include <cryptography/rsa/PublicKey.hxx>

#endif
