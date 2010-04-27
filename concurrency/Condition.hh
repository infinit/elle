//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/libraries/elle/concurrency/Condition.hh
//
// created       julien quintard   [sun mar  7 17:15:14 2010]
// updated       julien quintard   [mon apr 26 19:07:47 2010]
//

#ifndef ELLE_CONCURRENCY_CONDITION_HH
#define ELLE_CONCURRENCY_CONDITION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Meta.hh>

#include <elle/miscellaneous/Status.hh>

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
    class Condition:
      public Meta
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
