//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Transaction.hh
//
// created       julien quintard   [fri jun  3 12:19:03 2011]
// updated       julien quintard   [fri jun  3 13:53:46 2011]
//

#ifndef ETOILE_GEAR_TRANSACTION_HH
#define ETOILE_GEAR_TRANSACTION_HH

//
// ---------- includes --------------------------------------------------------
//

// XXX

namespace etoile
{
  namespace gear
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// a transaction represents an atomic event performed on a scope.
    ///
    template <const Operation O,
	      typename... T>
    class Transaction:
      public Event
    {
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <etoile/gear/Transaction.hxx>

#endif
