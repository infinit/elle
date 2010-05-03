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
// updated       julien quintard   [mon may  3 20:57:54 2010]
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

#include <elle/radix/Entity.hh>
#include <elle/radix/Meta.hh>
#include <elle/radix/Object.hh>
#include <elle/radix/Status.hh>
#include <elle/radix/Trace.hh>

#endif
