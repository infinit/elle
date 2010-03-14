//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/cryptography/Digest.hh
//
// created       julien quintard   [tue oct 30 01:16:28 2007]
// updated       julien quintard   [wed mar 10 20:29:07 2010]
//

#ifndef ELLE_CRYPTOGRAPHY_DIGEST_HH
#define ELLE_CRYPTOGRAPHY_DIGEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>
#include <elle/misc/Misc.hh>
#include <elle/archive/Archive.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace misc;
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
      public Entity,
      public Dumpable, public Archivable
    {
    public:
      //
      // interfaces
      //

      // entity
      embed(Entity, Digest);
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
