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
// updated       julien quintard   [sat sep  3 23:24:00 2011]
//

#ifndef ELLE_CONCURRENCY_CONDITION_HH
#define ELLE_CONCURRENCY_CONDITION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Entity.hh>

#include <elle/idiom/Close.hh>
# include <QWaitCondition>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace radix;

  namespace concurrency
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides a condition functionality.
    ///
    /// \todo XXX this class has to be written, especially for fibers.
    ///
    class Condition:
      public Entity
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
