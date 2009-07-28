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
// updated       julien quintard   [mon jul 27 08:28:42 2009]
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
      // constants
      //
      static const String		Class;

      //
      // constructors & destructors
      //
      Code()
      {
	Code::New(*this);
      }

      ~Code()
      {
	Code::Delete(*this);
      }

      //
      // interfaces
      //

      // object
      static Status	New(Code&);
      static Status	Delete(Code&);

      Code&		operator=(const Code&);
      Boolean		operator==(const Code&);
      Boolean		operator!=(const Code&);

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
				   const elle::crypto::Code&);
}

#endif
