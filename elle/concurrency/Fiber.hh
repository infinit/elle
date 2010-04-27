//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/libraries/elle/concurrency/Fiber.hh
//
// created       julien quintard   [sun mar 21 23:09:07 2010]
// updated       julien quintard   [tue apr 27 13:18:43 2010]
//

#ifndef ELLE_CONCURRENCY_FIBER_HH
#define ELLE_CONCURRENCY_FIBER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Meta.hh>

#include <elle/miscellaneous/Status.hh>
#include <elle/miscellaneous/Callback.hh>
#include <elle/miscellaneous/Closure.hh>

#include <elle/concurrency/Frame.hh>
#include <elle/concurrency/Event.hh>
#include <elle/concurrency/Resource.hh>
#include <elle/concurrency/Environment.hh>
#include <elle/concurrency/Phase.hh>

#include <elle/idiom/Close.hh>
# include <list>
# include <ucontext.h>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace miscellaneous;

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
    /// the fiber system is composed of a Fibers container that keeps
    /// fibers waiting for an event, a resource or a child fiber to
    /// complete its task.
    ///
    /// every fiber is composed of a stack frame, a low-leve context,
    /// a link to the parent fiber, the state of the fiber---active, awaken,
    /// suspended or completed---along with what the fiber is waiting for
    /// and a pointer to data that can be passed between the awakening
    /// and awaken fibers.
    ///
    /// note that every program is composed of at least one thread and
    /// one fiber, in which case concurrency control is needless.
    ///
    /// the fiber system works as follows. the current fiber decides to
    /// spawn a new fiber. the current fiber is put in the suspended state and
    /// added to the Fibers container as it will wait for the child fiber
    /// to complete.
    /// a fiber can also explicitely wait for an event/resource in which
    /// case it is suspended and put in the Fibers container.
    /// once a fiber is awaken, its state is changed but the fiber is
    /// not scheduled yet. indeed, once the current completes or is
    /// suspended, the scheduler is launched which takes the first awaken
    /// fiber and resumes it.
    ///
    class Fiber:
      public Meta,
      public Dumpable
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
      struct F
      {
	typedef std::list<Fiber*>		Container;
	typedef Container::iterator		Iterator;
	typedef Container::const_iterator	Scoutor;
      };

      struct C
      {
	typedef std::list<Fiber*>		Container;
	typedef Container::iterator		Iterator;
	typedef Container::const_iterator	Scoutor;
      };

      struct P
      {
	typedef std::vector< Callback<const Phase, Fiber*>* >	Container;
	typedef Container::iterator				Iterator;
	typedef Container::const_iterator			Scoutor;
      };

      //
      // static attributes
      //
      static F::Container	Fibers;

      static Fiber*		Application;
      static Fiber*		Current;

      static C::Container	Cache;

      static Void*		Trash;

      static P::Container	Phases;

      //
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();

      template <typename... T>
      static Status	Spawn(Closure<T...>&);
      template <typename... T>
      static Void	Launch(Closure<T...>*);

      template <typename T = Void>
      static Status	Wait(const Event&,
			     T*& = (T*&)Trash);
      template <typename T = Void>
      static Status	Awaken(const Event&,
			       T* = (T*)NULL);

      template <typename T = Void>
      static Status	Wait(const Resource*,
			     T*& = (T*&)Trash);
      template <typename T = Void>
      static Status	Awaken(const Resource*,
			       T* = (T*)NULL);

      static Status	Register(const Callback<const Phase, Fiber*>);
      static Status	Trigger(const Phase&);

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

      Void*		data;
    };
  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/concurrency/Fiber.hxx>

#endif
