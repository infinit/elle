//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/crypto/Signature.hh
//
// created       julien quintard   [tue oct 30 01:16:28 2007]
// updated       julien quintard   [thu jul 30 13:19:56 2009]
//

#ifndef ELLE_CRYPTO_SIGNATURE_HH
#define ELLE_CRYPTO_SIGNATURE_HH

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
    class Signature:
      public Entity,
      public Dumpable, public Archivable
    {
    public:
      //
      // constructors & destructors
      //
      Signature();
      ~Signature();

      //
      // interfaces
      //

      // entity
      Signature&	operator=(const Signature&);
      Boolean		operator==(const Signature&);
      Boolean		operator!=(const Signature&);

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
