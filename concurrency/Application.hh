//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Application.hh
//
// created       julien quintard   [mon mar 15 20:37:49 2010]
// updated       julien quintard   [wed mar 24 22:22:08 2010]
//

#ifndef ELLE_CONCURRENCY_APPLICATION_HH
#define ELLE_CONCURRENCY_APPLICATION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>

#include <elle/misc/Status.hh>

#include <elle/concurrency/Accord.hh>
#include <elle/concurrency/Event.hh>

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
    class Application
    {
    public:
      //
      // enumerations
      //
      enum Flags
	{
	  FlagNone,
	  FlagExit
	};

      //
      // static methods
      //
      static Status		Initialize();
      static Status		Clean();

      static Status		Setup(int,
				      char**);
      static Status		Exit(Status);

      static Status		Process();

      //
      // static attributes
      //
      static Accord			Control;

      static Flags			Flag;
      static ::QCoreApplication*	Core;
    };

  }
}

#endif
