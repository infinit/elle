//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [fri jun  3 11:01:57 2011]
//

#ifndef ETOILE_GEAR_SCOPE_HH
#define ETOILE_GEAR_SCOPE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/gear/Context.hh>
#include <etoile/gear/Chronicle.hh>
#include <etoile/gear/Nature.hh>
#include <etoile/gear/Actor.hh>
#include <etoile/gear/Operation.hh>

#include <etoile/path/Chemin.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a scope on which to operate in order to
    /// manipulate a file system object such as a file, directory or else.
    ///
    /// note that this class also maintains a static data structure holding
    /// the living scope according to their chemin so that whenever an
    /// actor tries to load a already loaded scope, the actor is simply
    /// attached to the scope.
    ///
    /// finally, note that anonymous scope are also kept. indeed, a file
    /// created by an application for instance does not have a chemin
    /// since the file has not be attached to the file system hierarchy
    /// yet. therefore, such scope are maintained in an anonymous data
    /// structure for as long as necessary.
    ///
    class Scope:
      public elle::Object
    {
    public:
      //
      // enumerations
      //
      enum State
	{
	  StateNone,
	  StateRefreshing,
	  StateDisclosing
	};

      //
      // types
      //
      struct S
      {
	struct O
	{
	  typedef std::map<const path::Chemin,
			   Scope*>			Container;
	  typedef Container::iterator			Iterator;
	  typedef Container::const_iterator		Scoutor;
	};

	struct A
	{
	  typedef std::list<Scope*>			Container;
	  typedef Container::iterator			Iterator;
	  typedef Container::const_iterator		Scoutor;
	};
      };

      struct A
      {
	typedef std::list<Actor*>			Container;
	typedef typename Container::iterator		Iterator;
	typedef typename Container::const_iterator	Scoutor;
      };

      //
      // static methods
      //
      static elle::Status	Initialize();
      static elle::Status	Clean();

      static elle::Status	Inclose(Scope*);
      static elle::Status	Acquire(const path::Chemin&,
					Scope*&);
      static elle::Status	Supply(Scope*&);
      static elle::Status	Relinquish(Scope*);
      static elle::Status	Annihilate(Scope*);

      static elle::Status	Show(const elle::Natural32 = 0);

      //
      // static attributes
      //
      struct				Scopes
      {
	static S::O::Container		Onymous;
	static S::A::Container		Anonymous;
      };

      //
      // constructors & destructors
      //
      Scope();
      Scope(const path::Chemin&);
      ~Scope();

      //
      // methods
      //
      elle::Status	Create();

      elle::Status	Locate(Actor*,
			       A::Iterator* = NULL);

      elle::Status	Attach(Actor*);
      elle::Status	Detach(Actor*);

      template <typename T>
      elle::Status	Use(T*&);

      elle::Status	Operate(const Operation);

      elle::Status	Shutdown();

      template <typename T>
      elle::Status	Refresh();
      template <typename T>
      elle::Status	Disclose();

      //
      // callbacks
      //
      elle::Status	Supervisor();

      //
      // interfaces
      //

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      State		state;
      elle::Timer	timer;

      path::Chemin	chemin;

      Context*		context;
      Chronicle*	chronicle;

      A::Container	actors;

      elle::Hurdle	hurdle;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <etoile/gear/Scope.hxx>

#endif
