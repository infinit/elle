#ifndef INFINIT_CRYPTOGRAPHY_PUBLICKEY_HH
# define INFINIT_CRYPTOGRAPHY_PUBLICKEY_HH

# include <utility>

# include <elle/Printable.hh>
# include <elle/attribute.hh>
# include <elle/concept/Uniquable.hh>
# include <elle/operator.hh>
# include <elle/serialization/Serializer.hh>
# include <elle/serialize/construct.hh>
# include <elle/types.hh>
# include <elle/utility/fwd.hh>

# include <cryptography/Clear.hh>
# include <cryptography/Code.hh>
# include <cryptography/Cryptosystem.hh>
# include <cryptography/Plain.hh>
# include <cryptography/Signature.hh>
# include <cryptography/oneway.hh>
# include <cryptography/fwd.hh>

ELLE_OPERATOR_RELATIONALS();

//
// ---------- Class -----------------------------------------------------------
//

namespace infinit
{
  namespace cryptography
  {
    /// Represent a public key in an asymmetric cryptosystem.
    ///
    /// Note that this class could have been designed as a mere interface
    /// implemented by every cryptosystem algorithm. Unfortunately, in
    /// order to be serializable, once must keep the algorithm so as to
    /// build the right instance through a factory. This is essentially
    /// what this class does along with forwarding the calls to the
    /// appropriate implementation.
    class PublicKey:
      public elle::concept::MakeUniquable<PublicKey>,
      public elle::Printable
    {
      /*-------------.
      | Construction |
      `-------------*/
    public:
      PublicKey(); // XXX[to deserialize]
      /// Construct a public key by providing its implementation.
      explicit
      PublicKey(std::unique_ptr<publickey::Interface>&& implementation);
# if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
      /// Construct a public key based on a given seed i.e in a deterministic
      /// way.
      explicit
      PublicKey(Seed const& seed);
# endif
      PublicKey(PublicKey const& other);
      PublicKey(PublicKey&& other);
      ELLE_SERIALIZE_CONSTRUCT_DECLARE(PublicKey);

      /*--------.
      | Methods |
      `--------*/
    public:
      /// Encrypt the given plain text.
      Code
      encrypt(Plain const& plain) const;
      /// Encrypt any serializable type.
      template <typename T>
      Code
      encrypt(T const& value) const;
      /// Return true if the given signature matches with the digest.
      elle::Boolean
      verify(Signature const& signature,
             Digest const& digest) const;
      /// Return true if the given signature matches with the plain text.
      elle::Boolean
      verify(Signature const& signature,
             Plain const& plain) const;
      /// Return true if the given signature matches with the serializable
      /// value.
      template <typename T>
      elle::Boolean
      verify(Signature const& signature,
             T const& value) const;
# if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
      /// Return the seed once rotated by the public key.
      Seed
      rotate(Seed const& seed) const;
      /// Return the seed once derived by the public key.
      Seed
      derive(Seed const& seed) const;
# endif
      /// Return the cryptosystem provided by this key.
      Cryptosystem
      cryptosystem() const;
      /// Return the public key's size in bytes.
      elle::Natural32
      size() const;
      /// Return the public key's length in bits.
      elle::Natural32
      length() const;
      /// Return the implementation.
      publickey::Interface const&
      implementation() const;

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
    | Printable |
    `-----------*/
    public:
      virtual
      void
      print(std::ostream& stream) const;

    /*--------------.
    | Serialization |
    `--------------*/
    public:
      PublicKey(elle::serialization::SerializerIn& serializer);
      void
      serialize(elle::serialization::Serializer& serializer);

    /*-------------.
    | Serializable |
    `-------------*/
    public:
      ELLE_SERIALIZE_FRIEND_FOR(PublicKey);

    /*--------.
    | Details |
    `--------*/
    private:
      ELLE_ATTRIBUTE(std::unique_ptr<publickey::Interface>, implementation);
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
    namespace publickey
    {
      /*----------.
      | Functions |
      `----------*/

      /// Return the factory used for building public key implementations given
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
    namespace publickey
    {
      /// Represent the public key interface to which every cryptosystem
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
        {}

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
        /// Return the public key's size in bytes.
        virtual
        elle::Natural32
        size() const = 0;
        /// Return the public key's length in bits.
        virtual
        elle::Natural32
        length() const = 0;
        /// Return the cryptosystem algorithm implemented by this public key.
        virtual
        Cryptosystem
        cryptosystem() const = 0;
        /// Encrypt the given plain text.
        virtual
        Code
        encrypt(Plain const& plain) const = 0;
        /// Return true if the given signature matches with the digest.
        virtual
        elle::Boolean
        verify(Signature const& signature,
               Digest const& digest) const = 0;
# if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
        /// Return the seed once rotated by the public key.
        virtual
        Seed
        rotate(Seed const& seed) const = 0;
        /// Return the seed once derived with the public key.
        virtual
        Seed
        derive(Seed const& seed) const = 0;
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

# include <cryptography/PublicKey.hxx>

#endif
