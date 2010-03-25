//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/misc/Closure.hh
//
// created       julien quintard   [thu mar 25 03:25:25 2010]
// updated       julien quintard   [thu mar 25 12:51:47 2010]
//

#ifndef ELLE_MISC_CLOSURE_HH
#define ELLE_MISC_CLOSURE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/misc/Callback.hh>

namespace elle
{
  namespace misc
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides closures i.e variables bound to a routine
    /// i.e callback or entrance.
    ///
    /// note that the constructors and duplicated in order to ensure
    /// type safety.
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

#include <elle/misc/Closure.hxx>

#endif
