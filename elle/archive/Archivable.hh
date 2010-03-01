//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/archive/Archivable.hh
//
// created       julien quintard   [tue jan 27 01:08:40 2009]
// updated       julien quintard   [mon mar  1 11:55:47 2010]
//

#ifndef ELLE_ARCHIVE_ARCHIVABLE_HH
#define ELLE_ARCHIVE_ARCHIVABLE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/misc/Misc.hh>

namespace elle
{
  using namespace core;
  using namespace misc;

  namespace archive
  {

//
// ---------- declarations ----------------------------------------------------
//

    ///
    /// this forward declares the Archive type.
    ///
    class Archive;

//
// ---------- class -----------------------------------------------------------
//

    ///
    /// this interface must be implemented for objects to be archived.
    ///
    class Archivable
    {
    public:
      //
      // methods
      //
      virtual Status	Serialize(Archive&) const
      {
	enter();

	fail("this method should never have been called");
      }

      virtual Status	Extract(Archive&)
      {
	enter();

	fail("this method should never have been called");
      }
    };

  }
}

#endif
