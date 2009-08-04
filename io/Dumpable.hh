//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/io/Dumpable.hh
//
// created       julien quintard   [mon feb  9 21:22:34 2009]
// updated       julien quintard   [tue aug  4 13:50:22 2009]
//

#ifndef ELLE_IO_DUMPABLE_HH
#define ELLE_IO_DUMPABLE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/misc/Status.hh>

namespace elle
{
  using namespace core;
  using namespace misc;

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
      // constructors & destructors
      //
      virtual ~Dumpable()
      {
      }

      //
      // methods
      //
      virtual Status	Dump(const Natural32 = 0) const = 0;
    };

  }
}

#endif
