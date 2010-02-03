//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/components/Access.hh
//
// created       julien quintard   [mon feb  1 19:22:40 2010]
// updated       julien quintard   [tue feb  2 18:53:04 2010]
//

#ifndef ETOILE_COMPONENTS_ACCESS_HH
#define ETOILE_COMPONENTS_ACCESS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/core/Core.hh>
#include <etoile/depot/Depot.hh>

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
    class Access
    {
    public:
      //
      // static methods
      //
      static Status	Open(context::Object*);
      static Status	Close(context::Object*);

      static Status	Retrieve(context::Object*,
				 const core::Subject&,
				 core::Permissions&,
				 Code&);
      static Status	Upgrade(context::Object*,
				const SecretKey&);
    };

  }
}

#endif
