//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/context/Rights.hh
//
// created       julien quintard   [tue feb  2 16:59:43 2010]
// updated       julien quintard   [tue feb  2 19:08:56 2010]
//

#ifndef ETOILE_CONTEXT_RIGHTS_HH
#define ETOILE_CONTEXT_RIGHTS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/core/Core.hh>

namespace etoile
{
  namespace context
  {

//
// ---------- structures ------------------------------------------------------
//

    ///
    /// XXX
    ///
    struct Rights
    {
    public:
      //
      // attributes
      //
      core::Permissions		permissions;
      SecretKey			key;
    };

  }
}

#endif
