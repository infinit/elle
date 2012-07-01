#ifndef ELLE_CRYPTOGRAPHY_SEED_HH
# define ELLE_CRYPTOGRAPHY_SEED_HH

# include <elle/types.hh>

# include <elle/radix/Object.hh>

# include <elle/cryptography/fwd.hh>
# include <elle/standalone/Region.hh>

# include <elle/idiom/Open.hh>

namespace elle
{

  using namespace radix;

  namespace cryptography
  {

    ///
    /// this class represents a seed which can especially be used for
    /// rotating keys.
    ///
    class Seed:
      public Object
    {
    public:
      //
      // constants
      //
      static const Natural32            Length;

      //
      // methods
      //
      Status            Generate();
      Status            Generate(const Natural32);

      Status            Rotate(const PrivateKey&,
                               Seed&) const;
      Status            Derive(const PublicKey&,
                               Seed&) const;

      //
      // interfaces
      //

      // object
      declare(Seed);
      Boolean           operator==(const Seed&) const;

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      standalone::Region            region;
    };

  }
}

#include <elle/cryptography/Seed.hxx>

#endif
