//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/components/Link.hh
//
// created       julien quintard   [fri aug 14 18:57:08 2009]
// updated       julien quintard   [mon may  3 17:55:10 2010]
//

#ifndef ETOILE_COMPONENTS_ETOILE_HH
#define ETOILE_COMPONENTS_ETOILE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/components/Object.hh>

#include <etoile/context/Link.hh>

#include <etoile/hole/Address.hh>

#include <etoile/kernel/Catalog.hh>

namespace etoile
{
  namespace components
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides functionalities for managing directories.
    ///
    class Link:
      public Object
    {
    public:
      //
      // static methods
      //
      static elle::Status	Create(context::Link*);
      static elle::Status	Load(context::Link*,
				     const hole::Address&);
      static elle::Status	Bind(context::Link*,
				     const path::Way&);
      static elle::Status	Resolve(context::Link*,
					path::Way&);
      static elle::Status	Discard(context::Link*);
      static elle::Status	Store(context::Link*);
      static elle::Status	Destroy(context::Link*);
    };

  }
}

#endif
