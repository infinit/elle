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
// updated       julien quintard   [mon aug 17 12:54:40 2009]
//

#ifndef ETOILE_COMPONENTS_CATALOG_HH
#define ETOILE_COMPONENTS_CATALOG_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/context/Context.hh>

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
      // methods
      //
      static Status	Open(context::Object&);
      static Status	Close(context::Object&);
    };

  }
}

#endif
