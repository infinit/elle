//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/archive/Archivable.hh
//
// created       julien quintard   [tue jan 27 01:08:40 2009]
// updated       julien quintard   [fri may 13 13:02:27 2011]
//

#ifndef ELLE_ARCHIVE_ARCHIVABLE_HH
#define ELLE_ARCHIVE_ARCHIVABLE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>
#include <elle/core/String.hh>

#include <elle/radix/Status.hh>

#include <elle/idiom/Close.hh>
# include <ostream>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace radix;

  namespace archive
  {

//
// ---------- declarations ----------------------------------------------------
//

    ///
    /// this forward declares the Archive class.
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
      virtual Status	Serialize(Archive&) const;
      virtual Status	Extract(Archive&);
    };

  }
}

#endif
