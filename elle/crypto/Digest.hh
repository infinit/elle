//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/crypto/Digest.hh
//
// created       julien quintard   [tue oct 30 01:16:28 2007]
// updated       julien quintard   [mon jul 27 08:29:01 2009]
//

#ifndef ELLE_CRYPTO_DIGEST_HH
#define ELLE_CRYPTO_DIGEST_HH

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
    class Digest:
      public Entity,
      public Dumpable, public Archivable
    {
    public:
      //
      // constants
      //
      static const String		Class;

      //
      // constructors & destructors
      //
      Digest()
      {
	Digest::New(*this);
      }

      ~Digest()
      {
	Digest::Delete(*this);
      }

      //
      // interfaces
      //

      // object
      static Status	New(Digest&);
      static Status	Delete(Digest&);

      Digest&		operator=(const Digest&);
      Boolean		operator==(const Digest&);
      Boolean		operator!=(const Digest&);

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

//
// ---------- operators -------------------------------------------------------
//

namespace std
{
  std::ostream&		operator<<(std::ostream&,
				   const elle::crypto::Digest&);
}

#endif
