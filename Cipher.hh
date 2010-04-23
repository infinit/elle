//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/cryptography/Cipher.hh
//
// created       julien quintard   [sat mar  7 21:34:17 2009]
// updated       julien quintard   [sun apr 18 12:37:56 2010]
//

#ifndef ELLE_CRYPTOGRAPHY_CIPHER_HH
#define ELLE_CRYPTOGRAPHY_CIPHER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>
#include <elle/archive/Archive.hh>

#include <elle/miscellaneous/Status.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace miscellaneous;
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
      // constants
      //
      static const Cipher		Null;

      //
      // interfaces
      //

      // entity
      declare(Entity, Cipher);
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
