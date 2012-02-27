//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       elle
//
// author        julien quintard   [mon apr 25 11:55:19 2011]
//

#ifndef ELLE_IO_PIECE_HH
#define ELLE_IO_PIECE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/String.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Object.hh>

namespace elle
{
  using namespace core;
  using namespace radix;

  namespace io
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a variable piece of a pattern-based path.
    ///
    class Piece:
      public Object
    {
    public:
      //
      // constructors & destructors
      //
      Piece();
      Piece(const String&,
            const String&);

      //
      // interfaces
      //

      // object
      declare(Piece);
      Boolean           operator==(const Piece&) const;

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      // archivable: nothing

      //
      // attributes
      //
      const String      name;
      const String      value;
    };

  }
}

#endif
