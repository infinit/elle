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
// updated       julien quintard   [wed mar 10 20:25:26 2010]
//

#ifndef ELLE_CONCURRENCY_THREAD_HH
#define ELLE_CONCURRENCY_THREAD_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>
#include <elle/misc/Misc.hh>

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
      static Thread*	Domain();
      template <typename T>
      static Thread*	Domain(T*);

      //
      // constructors & destructors
      //
      Thread()
      {
      }

      ~Thread()
      {
      }

      virtual Status Run() = 0;

      void run()
      {
	this->Run();

	// event loop.
	this->exec();
      }
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/concurrency/Thread.hxx>

#endif
