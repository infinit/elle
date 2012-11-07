#ifndef INFINIT_CRYPTOGRAPHY_KEYPAIR_HH
# define INFINIT_CRYPTOGRAPHY_KEYPAIR_HH

# include <elle/types.hh>

# include <elle/radix/Object.hh>

# include <cryptography/fwd.hh>
# include <cryptography/PublicKey.hh>
# include <cryptography/PrivateKey.hh>

# include <elle/idiom/Open.hh>

namespace infinit
{
  namespace cryptography
  {
    /// This class represents a cryptographic key pair _i.e_ a pair of public
    /// and private keys.
    ///
    /// A public key is noted with a capital 'K' while a private key is noted
    /// with a lower-case 'k'.
    class KeyPair
      : public elle::serialize::SerializableMixin<KeyPair>
    {
    public:
      //
      // static methods
      //
    public:
      /// Initialize the keypair contexts.
      static
      void
      initialize();
      /// Clean the keypair contexts.
      static
      void
      clean();
      /// Return a brand new, freshly generated key pair of the
      /// given length, or default one if not provided.
      static
      KeyPair
      generate(elle::Natural32 const length = Default::Length);
      // XXX[to remove if possible]
      static KeyPair const& null();

      //
      // constants
      //
      struct                            Default
      {
        static const elle::Natural32          Length;
      };

      //
      // constructors & destructors
      //
    public:
      KeyPair(elle::Natural32 length);
    private:
      KeyPair();

      //
      // methods
      //
    public:
      elle::Status            Rotate(const Seed&,
                               KeyPair&) const;

      //
      // interfaces
      //

      // object
      elle::Boolean           operator ==(const KeyPair&) const;

      // dumpable
      elle::Status            Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      PublicKey         K;
      PrivateKey        k;

      struct                    Contexts
      {
        static ::EVP_PKEY_CTX*  Generate;
      };
    };

  }
}

#include <cryptography/KeyPair.hxx>

#endif
