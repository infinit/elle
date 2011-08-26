//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/radix/Radix.hh
//
// created       julien quintard   [sun may  2 11:55:25 2010]
// updated       julien quintard   [fri aug 26 17:08:12 2011]
//

#ifndef ELLE_RADIX_RADIX_HH
#define ELLE_RADIX_RADIX_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/radix/Status.hh>

namespace elle
{
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
      static Status	Initialize();
      static Status	Clean();
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
#include <elle/radix/Morgue.hh>
#include <elle/radix/Object.hh>
#include <elle/radix/Parameters.hh>
#include <elle/radix/Set.hh>
#include <elle/radix/Status.hh>
#include <elle/radix/Trace.hh>
#include <elle/radix/Trait.hh>
#include <elle/radix/Variables.hh>

#endif
