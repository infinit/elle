#ifndef INFINIT_CRYPTOGRAPHY_KEYPAIR_HH
# define INFINIT_CRYPTOGRAPHY_KEYPAIR_HH

# include <elle/types.hh>
# include <elle/Printable.hh>
# include <elle/serialize/fwd.hh>

# include <cryptography/PublicKey.hh>
# include <cryptography/PrivateKey.hh>

namespace infinit
{
  namespace cryptography
  {
    /// Represent a cryptographic key pair _i.e_ a pair of public and
    /// private keys.
    ///
    /// Note that the public key is always written as a capital 'K'
    /// while a private key is noted with a lower-case 'k'.
    class KeyPair:
      public elle::Printable,
      public elle::io::Dumpable
    {
      /*------------------.
      | Static Attributes |
      `------------------*/
    public:
      struct Contexts
      {
        static ::EVP_PKEY_CTX* generate;
      };

      /*---------------.
      | Static Methods |
      `---------------*/
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
      /// given length.
      static
      KeyPair
      generate(elle::Natural32 const length);

      /*-------------.
      | Construction |
      `-------------*/
    public:
      KeyPair();
    private:
      KeyPair(::EVP_PKEY* key);

      //
      // methods
      //
    public:
      elle::Status            Rotate(const Seed&,
                               KeyPair&) const;

      // XXX
      // object
      elle::Boolean           operator ==(const KeyPair&) const;

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(KeyPair);
      // printable
      virtual
      void
      print(std::ostream& stream) const;

      //
      // attributes
      //
      PublicKey         K;
      PrivateKey        k;
    };

  }
}

# include <cryptography/KeyPair.hxx>

#endif
