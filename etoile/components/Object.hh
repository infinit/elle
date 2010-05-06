//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/components/Object.hh
//
// created       julien quintard   [fri aug 14 19:06:33 2009]
// updated       julien quintard   [tue may  4 11:00:46 2010]
//

#ifndef ETOILE_COMPONENTS_OBJECT_HH
#define ETOILE_COMPONENTS_OBJECT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/context/Object.hh>

#include <etoile/hole/Address.hh>

#include <etoile/wall/Status.hh>

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
      static elle::Status	Load(context::Object*,
				     const hole::Address&);
      static elle::Status	Information(context::Object*,
					    wall::Status&);
      static elle::Status	Store(context::Object*);
      static elle::Status	Discard(context::Object*);
      static elle::Status	Destroy(context::Object*);
    };

  }
}

#endif
