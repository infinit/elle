//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [tue mar  9 18:42:07 2010]
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
    /// XXX \todo to implement
    ///
    class Thread:
      public ::QThread,

      public Entity
    {
    public:
      //
      // static methods
      //
      static Thread*    Retrieve();
      template <typename T>
      static Thread*    Retrieve(T*);

      Status            Start();

      //
      // constructors & destructors
      //
      Thread() {};
      ~Thread() {};

      //
      // interface
      //
      virtual Status    Run() = 0;

      // qthread
      void              run();
    };

  }
}

#endif
