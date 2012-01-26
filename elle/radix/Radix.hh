//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [sun may  2 11:55:25 2010]
//

#ifndef ELLE_RADIX_RADIX_HH
#define ELLE_RADIX_RADIX_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/radix/Status.hh>

namespace elle
{
  ///
  /// the radix namespace contains fundamental classes on which all
  /// the other classes rely such as Object, Entity, Parameters etc.
  ///
  namespace radix
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides methods for managing the radix module.
    ///
    class Radix
    {
    public:
      //
      // static methods
      //
      static Status     Initialize();
      static Status     Clean();
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <elle/radix/Allege.hh>
#include <elle/radix/Arguments.hh>
#include <elle/radix/Entity.hh>
#include <elle/radix/Meta.hh>
#include <elle/radix/Object.hh>
#include <elle/radix/Parameters.hh>
#include <elle/radix/Set.hh>
#include <elle/radix/Status.hh>
#include <elle/radix/Trace.hh>
#include <elle/radix/Trait.hh>
#include <elle/radix/Variables.hh>

#endif
