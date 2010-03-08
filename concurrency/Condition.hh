//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Condition.hh
//
// created       julien quintard   [sun mar  7 17:15:14 2010]
// updated       julien quintard   [sun mar  7 19:36:21 2010]
//

#ifndef ELLE_CONCURRENCY_CONDITION_HH
#define ELLE_CONCURRENCY_CONDITION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <QWaitCondition>

#include <elle/core/Core.hh>
#include <elle/misc/Misc.hh>

namespace elle
{
  namespace concurrency
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides a condition functionality.
    ///
    class Condition
    {
    public:
      //
      // constants
      //
      static const Natural32		Timeout = 0;

      //
      // methods
      //
      Status		Wait(const Natural32 = Timeout);
      Status		Wake(/*options*/);

      //
      // attributes
      //
      ::QWaitCondition	condition;
    };

  }
}

#endif
