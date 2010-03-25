//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Fiber.hh
//
// created       julien quintard   [sun mar 21 23:09:07 2010]
// updated       julien quintard   [thu mar 25 22:31:53 2010]
//

#ifndef ELLE_CONCURRENCY_FIBER_HH
#define ELLE_CONCURRENCY_FIBER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>

#include <elle/misc/Status.hh>
#include <elle/misc/Closure.hh>

#include <elle/concurrency/Frame.hh>
#include <elle/concurrency/Event.hh>

#include <elle/idiom/Close.hh>
# include <list>
# include <ucontext.h>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace misc;

  namespace concurrency
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides the functionality to create fibers--i.e stack-based
    /// contexts---but also to jump from one another.
    ///
    /// this way, concurrent programming techniques can be applied without
    /// the burden of concurrency control but also without with performance
    /// impact of threading sub-systems.
    ///
    /// XXX \todo careful this class is not thread-safe! no accords are used!
    ///
    class Fiber:
      public Dumpable
    {
    public:
      //
      // constants
      //
      static const Natural32		Size;

      //
      // enumerations
      //
      enum State
	{
	  StateUnknown,
	  StateActive,
	  StateAwaken,
	  StateSuspended,
	  StateCompleted
	};

      //
      // types
      //
      typedef std::list<Fiber*>			Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // static attributes
      //
      static Container		Fibers;

      static Fiber		Application;
      static Fiber*		Handler;

      static Fiber*		Current;

      static Void*		Trash;

      //
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();

      template <typename... T>
      static Status	Spawn(Closure<T...>&);
      template <typename... T>
      static Void	Trigger(Closure<T...>*);

      template <typename T = Void>
      static Status	Wait(const Event&,
			     T*& = (T*&)Trash);
      template <typename T = Void>
      static Status	Awaken(const Event&,
			       T* = (T*)NULL);

      static Status	Schedule();

      static Status	Add(Fiber*);
      static Status	Locate(const Fiber*,
			       Iterator&);
      static Status	Locate(const Event&,
			       Iterator&);
      static Status	Remove(Fiber*);

      static Status	Show(const Natural32 = 0);

      //
      // constructors & destructors
      //
      Fiber();
      ~Fiber();

      //
      // methods
      //
      Status		Create(const Natural32 = Size);

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Frame*			frame;
      ::ucontext_t		context;

      State			state;
      Status			status;

      Event			event;

      Void*			data;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/concurrency/Fiber.hxx>

#endif
