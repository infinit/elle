//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/components/Link.hh
//
// created       julien quintard   [fri aug 14 18:57:08 2009]
// updated       julien quintard   [tue apr 20 10:15:16 2010]
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
      static Status	Create(context::Link*);
      static Status	Load(context::Link*,
			     const hole::Address&);
      static Status	Bind(context::Link*,
			     const path::Way&);
      static Status	Resolve(context::Link*,
				path::Way&);
      static Status	Store(context::Link*);
      static Status	Destroy(context::Link*);
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/journal/Journal.hh>

#include <etoile/user/User.hh>

#include <etoile/components/Rights.hh>
#include <etoile/components/Contents.hh>

#endif
