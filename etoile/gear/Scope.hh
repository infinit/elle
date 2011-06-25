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
// updated       julien quintard   [sat jun 25 13:38:54 2011]
//

#ifndef ETOILE_GEAR_SCOPE_HH
#define ETOILE_GEAR_SCOPE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/gear/Context.hh>
#include <etoile/gear/Identifier.hh>
#include <etoile/gear/Chronicle.hh>
#include <etoile/gear/Nature.hh>

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
      // constructors & destructors
      //
      Scope(const Nature);
      ~Scope();

      //
      // methods
      //
      elle::Status	Export();
      elle::Status	Import();

      //
      // interfaces
      //

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      Identifier	identifier;

      path::Chemin	chemin;

      Context*		context;
      Chronicle*	chronicle;
    };

  }
}

#endif
