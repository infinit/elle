//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Thread.hh
//
// created       julien quintard   [tue mar  9 18:42:07 2010]
// updated       julien quintard   [mon may  3 21:14:21 2010]
//

#ifndef ELLE_CONCURRENCY_THREAD_HH
#define ELLE_CONCURRENCY_THREAD_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/radix/Status.hh>
#include <elle/radix/Entity.hh>

#include <elle/idiom/Close.hh>
# include <QThread>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace radix;

  namespace concurrency
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Thread:
      public ::QThread,

      public Entity
    {
    public:
      //
      // static methods
      //
      static Thread*	Retrieve();
      template <typename T>
      static Thread*	Retrieve(T*);

      Status		Start();

      //
      // constructors & destructors
      //
      Thread() {}; // XXX
      ~Thread() {}; // XXX

      //
      // interface
      //
      virtual Status	Run() = 0;

      // qthread
      void		run();

      //
      // attributes
      //
      // XXX Wallet<T> wallet qui contient session, report etc. + T ou
      // un Get/Set(String -> Meta)

      // ensuite on fait Thread::Current qui opinte sur la courante.
      // Threads qui le contient toutes, dans Application

      // de cette maniere la seule globale est Application!
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/concurrency/Thread.hxx>

#endif
