//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/cryptography/Digest.hh
//
// created       julien quintard   [tue oct 30 01:16:28 2007]
// updated       julien quintard   [fri may 28 12:17:28 2010]
//

#ifndef ELLE_CRYPTOGRAPHY_DIGEST_HH
#define ELLE_CRYPTOGRAPHY_DIGEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/standalone/Region.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Object.hh>

#include <elle/archive/Archive.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace standalone;
  using namespace radix;
  using namespace archive;

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
      static const Digest		Null;

      //
      // interfaces
      //

      // object
      declare(Digest);
      Boolean		operator==(const Digest&) const;

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //
      Region		region;
    };

  }
}

#endif
