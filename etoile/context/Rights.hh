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
// updated       julien quintard   [sun mar 21 16:55:12 2010]
//

#ifndef ETOILE_CONTEXT_RIGHTS_HH
#define ETOILE_CONTEXT_RIGHTS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/core/Core.hh>

namespace etoile
{
  namespace context
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Rights:
      public Dumpable
    {
    public:
      //
      // interfaces
      //

      // dumpable
      Status			Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      core::Permissions		permissions;
      SecretKey			key;
    };

  }
}

#endif
