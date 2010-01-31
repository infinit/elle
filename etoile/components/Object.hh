//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/components/Object.hh
//
// created       julien quintard   [fri aug 14 19:06:33 2009]
// updated       julien quintard   [sat jan 30 17:32:11 2010]
//

#ifndef ETOILE_COMPONENTS_OBJECT_HH
#define ETOILE_COMPONENTS_OBJECT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/core/Core.hh>
#include <etoile/depot/Depot.hh>
#include <etoile/context/Context.hh>
#include <etoile/agent/Agent.hh>

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
    class Object
    {
    public:
      //
      // static methods
      //
      static Status	Load(context::Object*,
			     const hole::Address&);

      static Status	Rights(context::Object*,
			       core::Permissions&,
			       const core::Subject& = agent::Agent::Subject);
    };

  }
}

#endif
