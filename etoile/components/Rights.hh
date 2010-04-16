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
// updated       julien quintard   [wed apr 14 20:11:54 2010]
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
      static Status	Determine(context::Object*);
      static Status	Update(context::Object*,
			       const kernel::Permissions&);
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/user/User.hh>

#include <etoile/kernel/Token.hh>

#include <etoile/components/Access.hh>

#endif
