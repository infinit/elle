//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [mon oct 17 10:05:01 2011]
//

#ifndef ETOILE_NEST_HANDLE_HH
#define ETOILE_NEST_HANDLE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/Placement.hh>

namespace etoile
{
  namespace nest
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Handle
    {
    public:
      //
      // enumerations
      //
      enum Nature // XXX
        {
          NatureNew,
          NatureExisting
        };

      enum State
        {
          StateUnloaded,
          StateLoaded
        };

      //
      // constructors & destructors
      //
      Handle();
      Handle(const nucleus::Address&);
      Handle(const Placement&);

      // object
      declare(Handle);
      elle::Boolean     operator==(const Handle&) const;

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      State             state;

      Placement         placement;
      nucleus::Address  address;
    };

    /* XXX

1) on cree un quill, seam ou catalog

 -> aucune addresse
 -> on genere un handle avec un placement

2) on load un quill, seam ou catalog: addresse

 -> on a une addresse
 -> le nest fait une recherche sur addresse
 -> nous retourne un placement (soit existant soit nouveau)

3) on load un quill, seam ou catalog: placement

 -> c'est direct!

4) on unload un quill, sean ou catalog

 -> puisqu'il a ete loade, il doit avoir un placement

placement: nouveau
address: existant ET non-loaded
placement & address: existant ET loaded

     */

  }
}

#endif
