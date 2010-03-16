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
// updated       julien quintard   [tue mar 16 00:00:48 2010]
//

#ifndef ELLE_CONCURRENCY_THREAD_HH
#define ELLE_CONCURRENCY_THREAD_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>
#include <elle/misc/Misc.hh>

#include <elle/concurrency/Application.hh>

#include <elle/idiom/Close.hh>
# include <QThread>
#include <elle/idiom/Open.hh>

namespace elle
{
  namespace concurrency
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Thread:
      public ::QThread
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
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/concurrency/Thread.hxx>

#endif
