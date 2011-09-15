//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [sat mar  7 21:34:17 2009]
//

#ifndef ELLE_CRYPTOGRAPHY_CIPHER_HH
#define ELLE_CRYPTOGRAPHY_CIPHER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/standalone/Region.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Object.hh>

#include <elle/package/Archive.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace standalone;
  using namespace radix;
  using namespace package;

  namespace cryptography
  {

//
// ---------- types -----------------------------------------------------------
//

    ///
    /// this class represents a symmetrically ciphered text.
    ///
    class Cipher:
      public Object
    {
    public:
      //
      // constants
      //
      static const Cipher		Null;

      //
      // constructors & destructors
      //
      Cipher();

      //
      // interfaces
      //

      // object
      declare(Cipher);
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
