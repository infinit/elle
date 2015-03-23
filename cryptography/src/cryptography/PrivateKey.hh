#ifndef INFINIT_CRYPTOGRAPHY_PRIVATEKEY_HH
# define INFINIT_CRYPTOGRAPHY_PRIVATEKEY_HH

# include <utility>

# include <elle/concept/Uniquable.hh>
# include <elle/operator.hh>
# include <elle/serialization/Serializer.hh>
# include <elle/serialization/fwd.hh>
# include <elle/serialize/fwd.hh>
# include <elle/serialize/construct.hh>
# include <elle/types.hh>
# include <elle/utility/fwd.hh>

# include <cryptography/Clear.hh>
# include <cryptography/Cryptosystem.hh>
# include <cryptography/Seed.hh>
# include <cryptography/Signature.hh>
# include <cryptography/fwd.hh>
# include <cryptography/oneway.hh>

ELLE_OPERATOR_RELATIONALS();

//
// ---------- Class -----------------------------------------------------------
//

namespace infinit
{
  namespace cryptography
  {
    /// Represent a private key in a given cryptosystem, RSA for instance.
    ///
    /// Note that this class could have been designed as a mere interface
    /// implemented by every cryptosystem algorithm. Unfortunately, in
    /// order to be serializable, once must keep the algorithm so as to
    /// build the right instance through a factory. This is essentially
    /// what this class does along with forwarding the calls to the
    /// appropriate implementation.
    class PrivateKey:
      public elle::concept::MakeUniquable<PrivateKey>,
      public elle::Printable
    {
      /*-------------.
      | Construction |
      `-------------*/
    public:
      PrivateKey(); // XXX[deserialize]
      /// Construct a private key by providing its implementation.
      explicit
      PrivateKey(std::unique_ptr<privatekey::Interface>&& implementation);
# if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
      /// Construct a private key based on a given seed i.e in a deterministic
      /// way.
      explicit
      PrivateKey(Seed const& seed);
# endif
      PrivateKey(PrivateKey const& other);
      PrivateKey(PrivateKey&& other);
      ELLE_SERIALIZE_CONSTRUCT_DECLARE(PrivateKey);

      /*--------.
      | Methods |
      `--------*/
    public:
      /// Decrypt a code and returns the original clear text.
      ///
      /// Note that the code is, in practice, an archive containing both
      /// a temporarily-generated secret key and the plain text encrypted
      /// with the secret key.
      Clear
      decrypt(Code const& code) const;
      /// Decrypt a code and returns the given type, assuming the given type
      /// can be extracted from the clear, which should then be an archive.
      template <typename T>
      T
      decrypt(Code const& code) const;
      /// Return a signature of the given digest.
      Signature
      sign(Digest const& digest) const;
      /// Return a signature of the given plain text.
      Signature
      sign(Plain const& plain) const;
      /// Return a signature of any given serializable type.
      template <typename T>
      Signature
      sign(T const& value) const;
# if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
      /// Return the seed once derived by the private key.
      Seed
      derive(Seed const& seed) const;
      /// Return the seed once rotated by the private key.
      Seed
      rotate(Seed const& seed) const;
# endif
      /// Return the cryptosystem provided by this key.
      Cryptosystem
      cryptosystem() const;
      /// Return the private key's size in bytes.
      elle::Natural32
      size() const;
      /// Return the private key's length in bits.
      elle::Natural32
      length() const;
      /// Return the implementation.
      privatekey::Interface const&
      implementation() const;

      /*----------.
      | Operators |
      `----------*/
    public:
      elle::Boolean
      operator ==(PrivateKey const& other) const;
      elle::Boolean
      operator <(PrivateKey const& other) const;
      ELLE_OPERATOR_NO_ASSIGNMENT(PrivateKey);


    /*----------.
    | Printable |
    `----------*/
    public:
      void
      print(std::ostream& stream) const;

    /*--------------.
    | Serialization |
    `--------------*/
    public:
      PrivateKey(elle::serialization::SerializerIn& serializer);
      void
      serialize(elle::serialization::Serializer& serializer);

    /*-------------.
    | Serializable |
    `-------------*/
    public:
      ELLE_SERIALIZE_FRIEND_FOR(PrivateKey);

    /*--------.
    | Details |
    `--------*/
    private:
      ELLE_ATTRIBUTE(std::unique_ptr<privatekey::Interface>, implementation);
    };
  }
}

//
// ---------- Factory ---------------------------------------------------------
//

namespace infinit
{
  namespace cryptography
  {
    namespace privatekey
    {
      /*----------.
      | Functions |
      `----------*/

      /// Return the factory used for building private key implementations given
      /// a cryptosystem.
      elle::utility::Factory<Cryptosystem> const&
      factory();
    }
  }
}

//
// ---------- Interface -------------------------------------------------------
//

namespace infinit
{
  namespace cryptography
  {
    namespace privatekey
    {
      /// Represent the private key interface to which every cryptosystem
      /// implementation must comply.
      class Interface
        : public elle::Printable
        , public elle::serialize::Serializable<>
        , public elle::concept::Uniquable<>
        , public elle::serialization::VirtuallySerializable
      {
        /*-------------.
        | Construction |
        `-------------*/
      public:
        virtual
        ~Interface()
        {
        }

        /*----------.
        | Operators |
        `----------*/
      public:
        virtual
        elle::Boolean
        operator ==(Interface const& other) const = 0;
        virtual
        elle::Boolean
        operator <(Interface const& other) const = 0;

        /*--------.
        | Methods |
        `--------*/
      public:
        /// Clone the given implementation and return it.
        virtual
        Interface*
        clone() const = 0;
        /// Return the private key's size in bytes.
        virtual
        elle::Natural32
        size() const = 0;
        /// Return the private key's length in bits.
        virtual
        elle::Natural32
        length() const = 0;
        /// Return the cryptosystem algorithm implemented by this private key.
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
        /// Return a signature of the given digest.
        virtual
        Signature
        sign(Digest const& digest) const = 0;
# if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
        /// Return the seed once derived by the private key.
        virtual
        Seed
        derive(Seed const& seed) const = 0;
        /// Return the seed once rotated with the private key.
        virtual
        Seed
        rotate(Seed const& seed) const = 0;
# endif
      /*-------------.
      | Serializable |
      `-------------*/
      public:
        static constexpr char const* virtually_serializable_key = "algorithm";
        // Old school serialization also has a serialize method.
        using elle::serialization::VirtuallySerializable::serialize;
      };
    }
  }
}

# include <cryptography/PrivateKey.hxx>

#endif
