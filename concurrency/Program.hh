//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/libraries/elle/concurrency/Program.hh
//
// created       julien quintard   [mon mar 15 20:37:49 2010]
// updated       julien quintard   [mon apr 26 19:07:09 2010]
//

#ifndef ELLE_CONCURRENCY_PROGRAM_HH
#define ELLE_CONCURRENCY_PROGRAM_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Meta.hh>

#include <elle/miscellaneous/Status.hh>
#include <elle/miscellaneous/Callback.hh>

#include <elle/concurrency/Accord.hh>

#include <elle/idiom/Close.hh>
# include <QCoreApplication>
# include <list>
# include <signal.h>
#include <elle/idiom/Open.hh>

namespace elle
{
  namespace concurrency
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents the running program.
    ///
    class Program:
      public Meta
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

      static Status		Setup(const Natural32,
				      const Character*[],
				      Callback<>* = NULL,
				      Callback<>* = NULL);
      static Status		Launch();
      static Void		Signal(int);
      static Status		Exit();

      //
      // constructors & destructors
      //
      Program();
      ~Program();

      //
      // attributes
      //
      Accord			accord;

      Natural32			argc;
      const Character**		argv;

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
