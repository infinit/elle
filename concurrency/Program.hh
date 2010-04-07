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
// updated       julien quintard   [wed mar 31 21:34:36 2010]
//

#ifndef ELLE_CONCURRENCY_PROGRAM_HH
#define ELLE_CONCURRENCY_PROGRAM_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>

#include <elle/misc/Status.hh>
#include <elle/misc/Callback.hh>

#include <elle/concurrency/Accord.hh>

#include <elle/idiom/Close.hh>
# include <QCoreApplication>
# include <list>
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
    class Program
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
      static Status		Exit(Status);

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
      Status			status;

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
