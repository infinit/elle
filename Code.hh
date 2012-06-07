//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [tue oct 30 01:16:28 2007]
//

#ifndef ELLE_CRYPTOGRAPHY_CODE_HH
#define ELLE_CRYPTOGRAPHY_CODE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

#include <elle/standalone/Region.hh>

#include <elle/radix/Object.hh>

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
    class Code:
      public Object
    {
    public:
      //
      // constants
      //
      static const Code         Null;

      //
      // constructors & destructors
      //
      Code();

      //
      // methods
      //
      Status            Create(const Region&);

      //
      // interfaces
      //

      // object
      declare(Code);
      Boolean           operator==(const Code&) const;

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      // archivable
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
#include <elle/cryptography/Code.hxx>
