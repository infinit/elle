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
// updated       julien quintard   [fri sep  2 20:55:54 2011]
//

#ifndef ELLE_CONCURRENCY_PROGRAM_HH
#define ELLE_CONCURRENCY_PROGRAM_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/radix/Status.hh>
#include <elle/radix/Entity.hh>
#include <elle/radix/Parameters.hh>

#include <elle/concurrency/Callback.hh>
#include <elle/concurrency/Broker.hh>
#include <elle/concurrency/Signal.hh>

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
      // constants
      //
      static const Natural32	Quantum;

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

      static Status		Setup();
      static Status		Launch();
      static Void		Exception(int);
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
      ::QCoreApplication*	core;

      State			state;

      //
      // signals
      //
      struct
      {
	Signal<
	  Parameters<>
	  >			prolog;
	Signal<
	  Parameters<>
	  >			epilog;
      }				signal;
    };

//
// ---------- externs ---------------------------------------------------------
//

    extern Program*		program;

  }
}

#endif
