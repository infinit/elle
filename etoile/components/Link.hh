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
// updated       julien quintard   [thu may  5 16:16:56 2011]
//

#ifndef ETOILE_COMPONENTS_ETOILE_HH
#define ETOILE_COMPONENTS_ETOILE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/components/Object.hh>

#include <etoile/context/Link.hh>

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
				     const nucleus::Address&);
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
