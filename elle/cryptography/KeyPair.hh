#ifndef ELLE_CRYPTOGRAPHY_KEYPAIR_HH
# define ELLE_CRYPTOGRAPHY_KEYPAIR_HH

# include <elle/types.hh>

# include <elle/radix/Object.hh>

# include <elle/cryptography/fwd.hh>
# include <elle/cryptography/PublicKey.hh>
# include <elle/cryptography/PrivateKey.hh>

# include <elle/idiom/Open.hh>

namespace elle
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
      /// Initializes the keypair contexts.
      static
      void
      initialize();
      /// Cleans the keypair contexts.
      static
      void
      clean();

      /// Returns a brand new, freshly generated key pair of the
      /// given length, or default one if not provided.
      static
      KeyPair
      generate(elle::Natural32 const length = Default::Length);

      //
      // constants
      //
      struct                            Default
      {
        static const Natural32          Length;
      };

      static KeyPair const& null();

      static const String               Extension;

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
      Status            Generate();
      Status            Generate(const Natural32);

      Status            Create(const PublicKey&,
                               const PrivateKey&);

      Status            Rotate(const Seed&,
                               KeyPair&) const;

      //
      // interfaces
      //

      // object
      Boolean           operator ==(const KeyPair&) const;

      // dumpable
      Status            Dump(const Natural32 = 0) const;

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

#include <elle/cryptography/KeyPair.hxx>

#endif
