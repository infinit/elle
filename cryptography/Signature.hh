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

#include <elle/core/Natural.hh>
#include <elle/core/Boolean.hh>

#include <elle/standalone/Region.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Object.hh>

#include <elle/package/Archive.hh>

#include <elle/cryptography/Signature.hh>

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
      Status            Serialize(Archive&) const;
      Status            Extract(Archive&);

      //
      // attributes
      //
      Region            region;
    };

  }
}

#endif
