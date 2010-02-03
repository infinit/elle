//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/components/Catalog.hh
//
// created       julien quintard   [mon aug 17 11:44:37 2009]
// updated       julien quintard   [tue feb  2 19:11:37 2010]
//

#ifndef ETOILE_COMPONENTS_CATALOG_HH
#define ETOILE_COMPONENTS_CATALOG_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/depot/Depot.hh>
#include <etoile/context/Context.hh>

#include <etoile/components/Author.hh>
#include <etoile/components/Access.hh>
#include <etoile/components/Rights.hh>

namespace etoile
{
  namespace components
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Catalog
    {
    public:
      //
      // static methods
      //
      static Status	Open(context::Directory*);
      static Status	Close(context::Directory*);

      static Status	Lookup(context::Directory*,
			       const String&,
			       hole::Address&);
      static Status	Add(context::Directory*,
			    const String&,
			    const hole::Address&);
    };

  }
}

#endif
