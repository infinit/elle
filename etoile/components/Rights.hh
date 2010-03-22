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
// updated       julien quintard   [sun mar 21 18:12:05 2010]
//

#ifndef ETOILE_COMPONENTS_RIGHTS_HH
#define ETOILE_COMPONENTS_RIGHTS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/user/User.hh>
#include <etoile/context/Context.hh>

#include <etoile/components/Access.hh>

namespace etoile
{
  namespace components
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class makes it easy to concentrate the current user's permissions
    /// and access token but also to load those when the time is right
    ///
    class Rights
    {
    public:
      //
      // static methods
      //
      static Status	Determine(context::Object*);
    };

  }
}

#endif
