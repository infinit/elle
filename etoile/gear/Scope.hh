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
// updated       julien quintard   [fri jun 17 16:52:27 2011]
//

#ifndef ETOILE_GEAR_SCOPE_HH
#define ETOILE_GEAR_SCOPE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/gear/Identifier.hh>
#include <etoile/gear/Chronicle.hh>

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
    template <typename T>
    class Scope:
      public elle::Entity,
      public elle::Base<Scope>
    {
    public:
      //
      // constructors & destructors
      //
      Scope();
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
      // XXX

      //
      // attributes
      //
      Identifier	identifier;

      path::Chemin	chemin;

      T			context;
      Chronicle*	chronicle;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <etoile/gear/Scope.hxx>

#endif
