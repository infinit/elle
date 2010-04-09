//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/miscellaneous/Closure.hh
//
// created       julien quintard   [thu mar 25 03:25:25 2010]
// updated       julien quintard   [mon mar 29 23:10:43 2010]
//

#ifndef ELLE_MISCELLANEOUS_CLOSURE_HH
#define ELLE_MISCELLANEOUS_CLOSURE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/miscellaneous/Entrance.hh>

namespace elle
{
  namespace miscellaneous
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides closures i.e variables bound to a entrance.
    ///
    template <typename... T>
    class Closure
    {
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/miscellaneous/Closure.hxx>

#endif
