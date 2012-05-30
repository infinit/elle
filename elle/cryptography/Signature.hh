//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [tue oct 30 01:16:28 2007]
//

#ifndef ELLE_CRYPTOGRAPHY_SIGNATURE_HH
#define ELLE_CRYPTOGRAPHY_SIGNATURE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>
#include <elle/types.hh>

#include <elle/standalone/Region.hh>

#include <elle/radix/Object.hh>

#include <elle/cryptography/Signature.hh>

#include <elle/idiom/Open.hh>

namespace elle
{

  using namespace standalone;
  using namespace radix;

  namespace cryptography
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents an asymmetrically encrypted text.
    ///
    class Signature:
      public Object
    {
    public:
      //
      // constants
      //
      static const Signature            Null;

      //
      // constructors & destructors
      //
      Signature();

      //
      // interfaces
      //

      // object
      declare(Signature);
      Boolean           operator==(const Signature&) const;

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      // archivable
      // XXX
      //Status            Serialize(Archive&) const;
      //Status            Extract(Archive&);

      //
      // attributes
      //
      Region            region;
    };

  }
}

#endif
