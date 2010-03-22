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
// updated       julien quintard   [sun mar 21 16:58:38 2010]
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
      // methods
      //
      Status		Register(journal::Set::Container&);

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      core::Contents<core::Catalog>*	catalog;
    };

  }
}

#endif
