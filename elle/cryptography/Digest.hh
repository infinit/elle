#ifndef ELLE_CRYPTOGRAPHY_DIGEST_HH
# define ELLE_CRYPTOGRAPHY_DIGEST_HH

# include <elle/types.hh>

# include <elle/standalone/Region.hh>

# include <elle/radix/Object.hh>

namespace elle
{

  using namespace standalone;
  using namespace radix;

  namespace cryptography
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents an asymmetrically encrypted text.
    ///
    class Digest:
      public Object
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

      // archivable
      //Status            Serialize(Archive&) const;
      //Status            Extract(Archive&);

      //
      // attributes
      //
      Region            region;
    };

  }
}

#endif
