#ifndef ELLE_CRYPTOGRAPHY_DIGEST_HH
# define ELLE_CRYPTOGRAPHY_DIGEST_HH

# include <elle/types.hh>

# include <elle/standalone/Region.hh>
# include <elle/concept/Uniquable.hh>
# include <elle/serialize/HexadecimalArchive.hh>
# include <elle/radix/Object.hh>
# include <elle/idiom/Open.hh>

namespace elle
{
  namespace cryptography
  {

    ///
    /// this class represents an asymmetrically encrypted text.
    ///
    class Digest
      : public radix::Object
      , public elle::concept::MakeUniquable<
            Digest,
            elle::serialize::HexadecimalArchive
        >
    {
    public:
      //
      // constants
      //
      static const Digest               Null;

      //
      // constructors & destructors
      //
      Digest();

      //
      // interfaces
      //

      // object
      declare(Digest);
      Boolean           operator==(const Digest&) const;
      Boolean           operator<(const Digest&) const;

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      standalone::Region region;
    };

  }
}

#include <elle/cryptography/Digest.hxx>

#endif
