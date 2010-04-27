//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/libraries/elle/core/Core.hh
//
// created       julien quintard   [fri jan 30 16:14:39 2009]
// updated       julien quintard   [tue apr 27 12:12:18 2010]
//

#ifndef ELLE_CORE_CORE_HH
#define ELLE_CORE_CORE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/miscellaneous/Status.hh>

namespace elle
{
  using namespace miscellaneous;

  ///
  /// this namespace contains everything related to the core components
  /// such as basic types.
  ///
  namespace core
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides methods for managing the core module.
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

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Boolean.hh>
#include <elle/core/Character.hh>
#include <elle/core/Integer.hh>
#include <elle/core/Large.hh>
#include <elle/core/Meta.hh>
#include <elle/core/Natural.hh>
#include <elle/core/Null.hh>
#include <elle/core/Entity.hh>
#include <elle/core/Real.hh>
#include <elle/core/String.hh>
#include <elle/core/Type.hh>

#endif
