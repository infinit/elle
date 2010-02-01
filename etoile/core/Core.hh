//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Core.hh
//
// created       julien quintard   [mon feb 16 16:46:27 2009]
// updated       julien quintard   [sun jan 31 22:34:37 2010]
//

#ifndef ETOILE_CORE_CORE_HH
#define ETOILE_CORE_CORE_HH

//
// ---------- includes --------------------------------------------------------
//

// XXX
#include <etoile/core/ContentHashBlock.hh>
#include <etoile/core/Object.hh>
#include <etoile/core/PublicKeyBlock.hh>
#include <etoile/core/Permissions.hh>
#include <etoile/core/Token.hh>
#include <etoile/core/Proof.hh>
#include <etoile/core/Voucher.hh>
#include <etoile/core/Catalog.hh>
#include <etoile/core/Reference.hh>
#include <etoile/core/Data.hh>
#include <etoile/core/Genre.hh>
#include <etoile/core/Version.hh>
#include <etoile/core/Subject.hh>
#include <etoile/core/Component.hh>
#include <etoile/core/Contents.hh>
#include <etoile/core/State.hh>
#include <etoile/core/Offset.hh>
// XXX

namespace etoile
{
  ///
  /// XXX
  ///
  namespace core
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Core
    {
    public:
      //
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();
    };

  }
}

#endif
