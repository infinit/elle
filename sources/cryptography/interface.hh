#ifndef INFINIT_CRYPTOGRAPHY_INTERFACE_HH
# define INFINIT_CRYPTOGRAPHY_INTERFACE_HH

# include <cryptography/Plain.hh>
# include <cryptography/Code.hh>
# include <cryptography/Clear.hh>
# include <cryptography/Signature.hh>

# include <elle/operator.hh>
# include <elle/Printable.hh>
# include <elle/serialize/construct.hh>
# include <elle/serialize/Serializable.hh>
# include <elle/concept/Uniquable.hh>

# include <utility>
ELLE_OPERATOR_RELATIONALS();

namespace infinit
{
  namespace cryptography
  {
    namespace interface
    {
//
// ---------- Public Key ------------------------------------------------------
//

      /// Represent the public key interface to which every algorithm must
      /// comply.
      class PublicKey:
        public elle::Printable,
        public elle::serialize::Serializable<>,
        public elle::concept::Uniquable<>
      {
        /*-------------.
        | Construction |
        `-------------*/
      public:
        virtual
        ~PublicKey()
        {
        }

        /*----------.
        | Operators |
        `----------*/
      public:
        virtual
        elle::Boolean
        operator ==(PublicKey const& other) const = 0;
        virtual
        elle::Boolean
        operator <(PublicKey const& other) const = 0;

        /*--------.
        | Methods |
        `--------*/
      public:
        /// Clone the given implementation and return it.
        virtual
        PublicKey*
        clone() const = 0;
        /// Return the cryptosystem algorithm implemented by this public key.
        virtual
        Cryptosystem
        cryptosystem() const = 0;
        /// Encrypt the given plain text.
        virtual
        Code
        encrypt(Plain const& plain) const = 0;
        /// Return true if the given signature matches with the plain text.
        virtual
        elle::Boolean
        verify(Signature const& signature,
               Plain const& plain) const = 0;
        /// Decrypt the given code.
        virtual
        Clear
        decrypt(Code const& code) const = 0;
      };

//
// ---------- Private Key -----------------------------------------------------
//

      /// Represent the private key interface to which every algorithm must
      /// comply.
      class PrivateKey:
        public elle::Printable,
        public elle::serialize::Serializable<>,
        public elle::concept::Uniquable<>
      {
        /*-------------.
        | Construction |
        `-------------*/
      public:
        virtual
        ~PrivateKey()
        {
        }

        /*----------.
        | Operators |
        `----------*/
      public:
        virtual
        elle::Boolean
        operator ==(PrivateKey const& other) const = 0;
        virtual
        elle::Boolean
        operator <(PrivateKey const& other) const = 0;

        /*--------.
        | Methods |
        `--------*/
      public:
        /// Clone the given implementation and return it.
        virtual
        PrivateKey*
        clone() const = 0;
        /// Return the cryptosystem algorithm implemented by this public key.
        virtual
        Cryptosystem
        cryptosystem() const = 0;
        /// Decrypt a code and returns the original clear text.
        ///
        /// Note that the code is, in practice, an archive containing both
        /// a temporarily-generated secret key and the plain text encrypted
        /// with the secret key.
        virtual
        Clear
        decrypt(Code const& code) const = 0;
        /// Return a signature of the given plain text.
        virtual
        Signature
        sign(Plain const& plain) const = 0;
        /// Encrypt the given plain text with the private key.
        ///
        /// Although unusual, the private key can very well be used for
        /// encrypting in which case the public key would be used for
        /// decrypting.
        virtual
        Code
        encrypt(Plain const& plain) const = 0;
      };
    }
  }
}

# include <cryptography/interface.hxx>

#endif
