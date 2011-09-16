//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [sun mar 21 23:09:07 2010]
//

#ifndef ELLE_CONCURRENCY_FIBER_HH
#define ELLE_CONCURRENCY_FIBER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/system/Platform.hh>
#include <elle/core/Natural.hh>
#include <elle/core/Void.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Meta.hh>
#include <elle/radix/Parameters.hh>

#include <elle/concurrency/Callback.hh>
#include <elle/concurrency/Closure.hh>
#include <elle/concurrency/Event.hh>
#include <elle/concurrency/Resource.hh>
#include <elle/concurrency/Phase.hh>
#include <elle/concurrency/Frame.hh>
#include <elle/concurrency/Environment.hh>
#include <elle/concurrency/Timer.hh>

#include <elle/idiom/Close.hh>
# include <list>
# if INFINIT_UNIX
#  include <ucontext.h>
# elif INFINIT_WIN32
#  include <elle/thirdparty/ucontext-win32.hh>
# else
#  error "No makecontext support on this plateform."
# endif
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace radix;

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
    /// the burden of heavy concurrency control but also without the
    /// performance impact of threading sub-systems.
    ///
    /// the fiber system is composed of a fiber container that keeps
    /// fibers waiting for an event, a resource or a child fiber to
    /// complete a task.
    ///
    /// every fiber is composed of a stack frame, a low-level context,
    /// a link to the parent fiber, the state of the fiber---active, awaken,
    /// suspended or completed---along with what the fiber is waiting for
    /// and a pointer to data that can be passed between the awakening
    /// and awaken fibers.
    ///
    /// note that every application is composed of at least one fiber
    /// in which case concurrency control is needless.
    ///
    /// the fiber system works as follows. the current fiber decides to
    /// spawn a new fiber. the current fiber is put in the suspended state and
    /// added to the fiber container as it will wait for the child fiber
    /// to complete.
    ///
    /// a fiber can also explicitly wait for an event/resource in which
    /// case it is suspended and put in the fiber container.
    ///
    /// once a fiber is awaken, its state is changed but the fiber is
    /// not scheduled yet. indeed, once the current fiber completes or is
    /// suspended, the scheduler is launched which takes the first awaken
    /// fiber and resumes it.
    ///
    class Fiber:
      public Entity
    {
    public:
      //
      // constants
      //
      static const Natural32		Size;
      static const Natural32		Capacity;

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


      enum Type
	{
	  TypeNone,
	  TypeEvent,
	  TypeResource,
	  TypeFiber
	};

      //
      // structures
      //

      // fibers
      struct F
      {
	typedef std::list<Fiber*>			Container;
	typedef Container::iterator			Iterator;
	typedef Container::const_iterator		Scoutor;
      };

      // cache
      struct C
      {
	typedef std::list<Fiber*>			Container;
	typedef Container::iterator			Iterator;
	typedef Container::const_iterator		Scoutor;
      };

      // phases
      struct P
      {
	typedef std::vector<
	  Callback<
	    Status,
	    Parameters<Phase, Fiber*> >* >              Container;
	typedef Container::iterator			Iterator;
	typedef Container::const_iterator		Scoutor;
      };

      //
      // static attributes
      //
      static F::Container	Fibers;

      static Fiber*		Program;
      static Fiber*		Current;

      static C::Container	Cache;

      static Meta*		Trash;

      static P::Container	Phases;

      //
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();

      template <typename... T>
      static Status	Spawn(Closure<Status, T...>&);
      template <typename... T>
      static Void	Launch(Closure<Status, T...>*);

      template <typename T = Meta>
      static Status	Wait(const Event&,
			     T*& = reinterpret_cast<T*&>(Trash));
      template <typename T = Meta>
      static Status	Awaken(const Event&,
			       T* = static_cast<T*>(NULL));

      template <typename T = Meta>
      static Status	Wait(const Resource*,
			     T*& = reinterpret_cast<T*&>(Trash));
      template <typename T = Meta>
      static Status	Awaken(const Resource*,
			       T* = static_cast<T*>(NULL));

      static Status	Sleep(const Natural32);

      static Status	Register(const
				   Callback<
				     Status,
				     Parameters<Phase, Fiber*>
				     >);
      static Status	Trigger(Phase);

      static Status	Schedule();

      static Status	New(Fiber*&);
      static Status	Delete(Fiber*);

      static Status	Add(Fiber*);
      static Status	Remove(Fiber*);

      static Status	Locate(const Event&,
			       F::Iterator&);
      static Status	Locate(const Resource*,
			       F::Iterator&);

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
      // callbacks
      //
      Status		Timeout();

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Fiber*		link;

      Frame*		frame;
      ::ucontext_t	context;

      State		state;

      Type		type;
      union
      {
	Event*		event;
	const Resource*	resource;
      };

      Environment*	environment;

      Meta*		data;

      Timer*		timer;
    };
  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/concurrency/Fiber.hxx>

#endif
