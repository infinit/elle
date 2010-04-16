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
// updated       julien quintard   [thu apr 15 20:53:38 2010]
//

#ifndef ETOILE_COMPONENTS_OBJECT_HH
#define ETOILE_COMPONENTS_OBJECT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/context/Object.hh>

#include <etoile/hole/Address.hh>

namespace etoile
{
  namespace components
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides basic functionalities for handling objects.
    ///
    class Object
    {
    public:
      //
      // static methods
      //
      static Status	Load(context::Object*,
			     const hole::Address&);
      static Status	Information(context::Object*,
				    wall::State&);
      static Status	Store(context::Object*);
      static Status	Destroy(context::Object*);
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/user/User.hh>

#include <etoile/depot/Depot.hh>

#include <etoile/components/Access.hh>

#include <etoile/journal/Journal.hh>

#include <etoile/wall/State.hh>

#endif
