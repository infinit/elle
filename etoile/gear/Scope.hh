//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Scope.hh
//
// created       julien quintard   [fri jun  3 11:01:57 2011]
// updated       julien quintard   [sun jul 31 11:14:47 2011]
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
    /// a scope represents a set of operations represented by a chronicle
    /// applied onto a context, leading to a living state.
    ///
    /// should a conflict be detected upon publishing the state's modified
    /// constituent blocks, the system would be able to fetch the latest
    /// version of the context and re-apply the set of operations.
    ///
    class Scope:
      public elle::Object
    {
    public:
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
      // methods
      //
      static elle::Status	Acquire(const path::Chemin&,
					Scope*&);
      static elle::Status	Supply(Scope*&);
      static elle::Status	Relinquish(Scope*);

      static elle::Status	Show(const elle::Natural32 = 0);

      //
      // static attributes
      //
      struct			Scopes
      {
	static S::O::Container	Onymous;
	static S::A::Container	Anonymous;
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
      elle::Status	Locate(Actor*,
			       A::Iterator* = NULL);

      elle::Status	Attach(Actor*);
      elle::Status	Detach(Actor*);

      template <const Nature, typename T>
      elle::Status	Use(T*&);

      elle::Status	Operate(const Operation);

      //
      // interfaces
      //

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      path::Chemin	chemin;

      Context*		context;
      Chronicle*	chronicle;

      A::Container	actors;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <etoile/gear/Scope.hxx>

#endif
