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
// updated       julien quintard   [wed mar 24 22:23:03 2010]
//

#ifndef ELLE_CONCURRENCY_CONDITION_HH
#define ELLE_CONCURRENCY_CONDITION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>

#include <elle/misc/Status.hh>

#include <elle/idiom/Close.hh>
# include <QWaitCondition>
#include <elle/idiom/Open.hh>

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
    /// XXX \todo this class has to be written!
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
