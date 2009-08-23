//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/context/Directory.hh
//
// created       julien quintard   [fri aug 14 23:13:51 2009]
// updated       julien quintard   [sat aug 22 02:20:24 2009]
//

#ifndef ETOILE_CONTEXT_DIRECTORY_HH
#define ETOILE_CONTEXT_DIRECTORY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/context/Object.hh>

namespace etoile
{
  namespace context
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Directory:
      public Object
    {
    public:
      //
      // constructors & destructors
      Directory();
      Directory(const Directory&);
      ~Directory();

      //
      // attributes
      //
      core::Catalog*	catalog;
    };

  }
}

#endif
