//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Gear.hh
//
// created       julien quintard   [fri jun  3 10:58:40 2011]
// updated       julien quintard   [thu jul 28 12:46:21 2011]
//

#ifndef ETOILE_GEAR_GEAR_HH
#define ETOILE_GEAR_GEAR_HH

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/gear/Identifier.hh>
#include <etoile/gear/Scope.hh>

#include <elle/idiom/Close.hh>
# include <map>
#include <elle/idiom/Open.hh>

namespace etoile
{
  ///
  /// this namespace contains everything for recording operations on
  /// the file system so that such operations could be replayed for instance.
  ///
  namespace gear
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Gear
    {
    public:
      //
      // types
      //
      struct C
      {
	typedef std::map<const path::Chemin,
			 const Identifier>		Container;
	typedef Container::iterator			Iterator;
	typedef Container::const_iterator		Scoutor;
      };

      struct S
      {
	typedef std::map<const Identifier,
			 Scope*>			Container;
	typedef Container::iterator			Iterator;
	typedef Container::const_iterator		Scoutor;
      };

      //
      // static methods
      //

      // XXX to do these two!
      template <typename T>
      static elle::Status	New(const path::Chemin&,
				    T*&);
      template <typename T>
      static elle::Status	Delete(T*);

      static elle::Status	Add(const Identifier&,
				    Scope*);
      static elle::Status	Remove(const Identifier&);

      static elle::Status	Select(const Identifier&,
				       Scope*&);

      static elle::Status	Initialize();
      static elle::Status	Clean();

      static elle::Status	Show(const elle::Natural32 = 0);

      //
      // static attributes
      //
      static C::Container	Chemins;
      static S::Container	Scopes;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <etoile/gear/Gear.hxx>

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/gear/Action.hh>
#include <etoile/gear/Actor.hh>
#include <etoile/gear/Chronicle.hh>
#include <etoile/gear/Context.hh>
#include <etoile/gear/Directory.hh>
#include <etoile/gear/Event.hh>
#include <etoile/gear/File.hh>
#include <etoile/gear/Identifier.hh>
#include <etoile/gear/Link.hh>
#include <etoile/gear/Nature.hh>
#include <etoile/gear/Object.hh>
#include <etoile/gear/Operation.hh>
#include <etoile/gear/Scope.hh>
#include <etoile/gear/State.hh>
#include <etoile/gear/Transaction.hh>
#include <etoile/gear/Transcript.hh>

#endif
