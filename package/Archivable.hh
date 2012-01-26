//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [tue jan 27 01:08:40 2009]
//

#ifndef ELLE_PACKAGE_ARCHIVABLE_HH
#define ELLE_PACKAGE_ARCHIVABLE_HH

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

  namespace package
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// this forward declares Archive.
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
      // virtual methods
      //
      virtual Status    Serialize(Archive&) const;
      virtual Status    Extract(Archive&);
    };

  }
}

#endif
