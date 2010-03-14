//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/cryptography/Cipher.hh
//
// created       julien quintard   [sat mar  7 21:34:17 2009]
// updated       julien quintard   [wed mar 10 20:28:09 2010]
//

#ifndef ELLE_CRYPTOGRAPHY_CIPHER_HH
#define ELLE_CRYPTOGRAPHY_CIPHER_HH

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
// ---------- types -----------------------------------------------------------
//

    ///
    /// this class represents a symmetrically ciphered text.
    ///
    class Cipher:
      public Entity,
      public Dumpable, public Archivable
    {
    public:
      //
      // interfaces
      //

      // entity
      embed(Entity, Cipher);
      Boolean		operator==(const Cipher&) const;

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
