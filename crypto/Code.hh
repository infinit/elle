//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/crypto/Code.hh
//
// created       julien quintard   [tue oct 30 01:16:28 2007]
// updated       julien quintard   [mon aug  3 20:59:28 2009]
//

#ifndef ELLE_CRYPTO_CODE_HH
#define ELLE_CRYPTO_CODE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>
#include <elle/misc/Misc.hh>
#include <elle/archive/Archive.hh>

namespace elle
{
  using namespace core;
  using namespace misc;
  using namespace archive;

  namespace crypto
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents an asymmetrically encrypted text.
    ///
    class Code:
      public Entity,
      public Dumpable, public Archivable
    {
    public:
      //
      // interfaces
      //

      // entity
      Code&		operator=(const Code&);
      Boolean		operator==(const Code&) const;
      Boolean		operator!=(const Code&) const;

      // dumpable
      Status		Dump(const Natural32 = 0);

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
