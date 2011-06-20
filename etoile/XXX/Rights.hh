//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/components/Rights.hh
//
// created       julien quintard   [tue feb  2 16:54:18 2010]
// updated       julien quintard   [thu may  5 16:17:44 2011]
//

#ifndef ETOILE_COMPONENTS_RIGHTS_HH
#define ETOILE_COMPONENTS_RIGHTS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/context/Object.hh>

namespace etoile
{
  namespace components
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class makes it easy to concentrate the current user's permissions
    /// and access the token but also to load those when the time is right.
    ///
    class Rights
    {
    public:
      //
      // static methods
      //
      static elle::Status	Determine(context::Object*);
      static elle::Status	Update(context::Object*,
				       const nucleus::Permissions&);
    };

  }
}

#endif
