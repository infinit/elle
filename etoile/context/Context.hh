//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/context/Context.hh
//
// created       julien quintard   [fri aug 14 22:36:10 2009]
// updated       julien quintard   [mon aug 17 12:25:24 2009]
//

#ifndef ETOILE_CONTEXT_CONTEXT_HH
#define ETOILE_CONTEXT_CONTEXT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/core/Core.hh>

namespace etoile
{
  namespace context
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Context:
      public Entity,
      public Dumpable
    {
    public:
      //
      // constructors & destructors
      //
      Context();

      //
      // interfaces
      //

      // entity
      Context&		operator=(const Context&);
      Boolean		operator==(const Context&) const;
      Boolean		operator!=(const Context&) const;

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      core::Component	component;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/context/Object.hh>

#endif
