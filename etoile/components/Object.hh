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
// updated       julien quintard   [thu may  5 16:18:20 2011]
//

#ifndef ETOILE_COMPONENTS_OBJECT_HH
#define ETOILE_COMPONENTS_OBJECT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/context/Object.hh>

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
				     const nucleus::Address&);
      static elle::Status	Information(context::Object*,
					    wall::Status&);
      static elle::Status	Store(context::Object*);
      static elle::Status	Discard(context::Object*);
      static elle::Status	Destroy(context::Object*);
    };

  }
}

#endif
