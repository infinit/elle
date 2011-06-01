//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Program.hh
//
// created       julien quintard   [mon mar 15 20:37:49 2010]
// updated       julien quintard   [mon may 30 22:10:43 2011]
//

#ifndef ELLE_CONCURRENCY_PROGRAM_HH
#define ELLE_CONCURRENCY_PROGRAM_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/radix/Status.hh>
#include <elle/radix/Entity.hh>

#include <elle/concurrency/Callback.hh>
#include <elle/concurrency/Accord.hh>
#include <elle/concurrency/Broker.hh>

#include <elle/idiom/Close.hh>
# include <QCoreApplication>
# include <QAbstractEventDispatcher>
# include <list>
# include <signal.h>
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
    /// this class represents the running program.
    ///
    class Program:
      public Entity
    {
    public:
      //
      // enumerations
      //
      enum State
	{
	  StateUnknown,
	  StateActive,
	  StateStopped
	};

      //
      // static methods
      //
      static Status		Initialize();
      static Status		Clean();

      static Status		Setup(Callback<>* = NULL,
				      Callback<>* = NULL);
      static Status		Launch();
      static Void		Signal(int);
      static Status		Exit();

      static Status		Attach(Broker&);
      static Status		Detach(Broker&);

      //
      // constructors & destructors
      //
      Program();
      ~Program();

      //
      // attributes
      //
      Accord			accord;

      ::QCoreApplication*	core;

      State			state;

      Callback<>*		prolog;
      Callback<>*		epilog;
    };

//
// ---------- externs ---------------------------------------------------------
//

    extern Program*		program;

  }
}

#endif
