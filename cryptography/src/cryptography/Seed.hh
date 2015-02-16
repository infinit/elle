#if defined(ELLE_CRYPTOGRAPHY_ROTATION)

# ifndef INFINIT_CRYPTOGRAPHY_SEED_HH
#  define INFINIT_CRYPTOGRAPHY_SEED_HH

#  include <cryptography/fwd.hh>

#  include <elle/types.hh>
#  include <elle/attribute.hh>
#  include <elle/operator.hh>
#  include <elle/Buffer.hh>
#  include <elle/Printable.hh>
#  include <elle/serialize/fwd.hh>
#  include <elle/utility/fwd.hh>

//
// ---------- Class -----------------------------------------------------------
//

namespace infinit
{
  namespace cryptography
  {
    /// Represent some information from which cryptographic objects can be
    /// generated in a deterministic way.
    ///
    /// For instance, given the seed 'abcdefgh', one is guaranteed to always
    /// generate the same KeyPair.
    class Seed:
      public elle::Printable
    {
      /*---------------.
      | Static Methods |
      `---------------*/
    public:
      /// Return a randomly generated seed suitable for the given key pair.
      static
      Seed
      generate(KeyPair const& pair);

      /*-------------.
      | Construction |
      `-------------*/
    public:
      Seed(); // XXX[to deserialize]
      /// Construct a seed by providing its implementation.
      explicit
      Seed(std::unique_ptr<seed::Interface>&& implementation);
      Seed(Seed const& seed);
      Seed(Seed&& other);
      ELLE_SERIALIZE_CONSTRUCT_DECLARE(Seed);

      /*--------.
      | Methods |
      `--------*/
    public:
      /// Return the cryptosystem provided by this key.
      Cryptosystem
      cryptosystem() const;
      /// Return the implementation.
      seed::Interface const&
      implementation() const;

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
      ELLE_ATTRIBUTE(std::unique_ptr<seed::Interface>, implementation);
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
    namespace seed
    {
      /*----------.
      | Functions |
      `----------*/

      /// Return the factory used for building seed implementations given
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
    namespace seed
    {
      /// Represent the seed interface to which every cryptosystem
      /// implementation must comply.
      class Interface:
        public elle::Printable,
        public elle::serialize::Serializable<>
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

        /*--------.
        | Methods |
        `--------*/
      public:
        /// Clone the given implementation and return it.
        virtual
        Interface*
        clone() const = 0;
        /// Return the cryptosystem algorithm of the seed.
        virtual
        Cryptosystem
        cryptosystem() const = 0;
      };
    }
  }
}

#  include <cryptography/Seed.hxx>

# endif

#endif
