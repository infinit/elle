#ifndef INFINIT_CRYPTOGRAPHY_SEED_HH
# define INFINIT_CRYPTOGRAPHY_SEED_HH

# include <elle/types.hh>
# include <elle/radix/Object.hh>
# include <elle/standalone/Region.hh>

# include <cryptography/fwd.hh>

# include <elle/idiom/Open.hh>

namespace infinit
{
  namespace cryptography
  {

    ///
    /// this class represents a seed which can especially be used for
    /// rotating keys.
    ///
    class Seed:
      public elle::radix::Object
    {
    public:
      //
      // constants
      //
      static const elle::Natural32            Length;

      //
      // methods
      //
      elle::Status            Generate();
      elle::Status            Generate(const elle::Natural32);

      elle::Status            Rotate(const PrivateKey&,
                               Seed&) const;
      elle::Status            Derive(const PublicKey&,
                               Seed&) const;

      //
      // interfaces
      //

      // object
      declare(Seed);
      elle::Boolean           operator==(const Seed&) const;

      // dumpable
      elle::Status            Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      elle::standalone::Region            region;
    };

  }
}

#include <cryptography/Seed.hxx>

#endif
