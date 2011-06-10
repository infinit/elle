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
// updated       julien quintard   [fri jun  3 11:40:59 2011]
//

#ifndef ETOILE_GEAR_SCOPE_HH
#define ETOILE_GEAR_SCOPE_HH

//
// ---------- includes --------------------------------------------------------
//

// XXX

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
      // constructors & destructors
      //
      Scope(const Nature,
	    const path::Route&,
	    const nucleus::Address&);

      //
      // attributes
      //
      Nature		nature;

      path::Route	route;
      nucleus::Address	address;

      Context		context;
      Chronicle		chronicle;
      State		state;

      // XXX peut etre faire une classe speciale genre Subject qui
      //  definit l'auteur courant et ses perms sur le scope car en fonction
      //  des modifs les perms changent (donc rapport au state)
      // XXX Rights*			rights;
      // XXX nucleus::Author*		author;
    };

  }
}

#endif
