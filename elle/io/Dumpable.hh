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
// updated       julien quintard   [wed mar 10 20:31:27 2010]
//

#ifndef ELLE_IO_DUMPABLE_HH
#define ELLE_IO_DUMPABLE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/misc/Status.hh>
#include <elle/misc/Report.hh>
#include <elle/misc/Maid.hh>

#include <elle/idiom/Open.hh>

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
      virtual Status	Dump(const Natural32 = 0) const
      {
	enter();

	fail("this method should never have been called");
      }
    };

  }
}

#endif
