//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/io/Dumpable.hh
//
// created       julien quintard   [mon feb  9 21:22:34 2009]
// updated       julien quintard   [thu jun 23 13:02:15 2011]
//

#ifndef ELLE_IO_DUMPABLE_HH
#define ELLE_IO_DUMPABLE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>
#include <elle/core/String.hh>

#include <elle/radix/Status.hh>

namespace elle
{
  using namespace core;
  using namespace radix;

  namespace io
  {

//
// ---------- class -----------------------------------------------------------
//

    ///
    /// this interface must be implemented by dumpable classes.
    ///
    class Dumpable
    {
    public:
      //
      // constants
      //
      static const String		Shift;

      //
      // constructors & destructors
      //
      virtual ~Dumpable()
      {
      }

      //
      // methods
      //
      virtual Status	Dump(const Natural32 = 0) const;
    };

  }
}

#endif
