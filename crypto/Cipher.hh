//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       GPL
//
// file          /home/mycure/infinit/elle/crypto/Cipher.hh
//
// created       julien quintard   [sat mar  7 21:34:17 2009]
// updated       julien quintard   [wed mar 11 16:05:25 2009]
//

#ifndef ELLE_CRYPTO_CIPHER_HH
#define ELLE_CRYPTO_CIPHER_HH

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
// ---------- types -----------------------------------------------------------
//

    ///
    /// this class represents a symmetrically ciphered text.
    ///
    class Cipher:
      public Object,
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
      Cipher()
      {
	Cipher::New(*this);
      }

      ~Cipher()
      {
	Cipher::Delete(*this);
      }

      //
      // interfaces
      //

      // object
      static Status	New(Cipher&);
      static Status	Delete(Cipher&);

      Cipher&		operator=(const Cipher&);
      Boolean		operator==(const Cipher&);
      Boolean		operator!=(const Cipher&);

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
