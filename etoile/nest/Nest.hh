//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [mon oct 17 09:56:52 2011]
//

#ifndef ETOILE_NEST_NEST_HH
#define ETOILE_NEST_NEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/Nucleus.hh>
#include <XXX/Handle.hh>

#include <etoile/nest/Pod.hh>

#include <map>

namespace etoile
{
  namespace nest
  {

//
// ---------- classes ---------------------------------------------------------
//

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

    ///
    /// XXX
    ///
    class Nest
    {
    public:
      //
      // types
      //
      struct P
      {
        typedef std::map<const nucleus::Placement, Pod*>        Container;
        typedef typename Container::iterator                    Iterator;
      };

      struct A
      {
        typedef std::map<const nucleus::Address, Pod*>          Container;
        typedef typename Container::iterator                    Iterator;
      };

      //
      // static methods
      //
      static elle::Status       Attach(nucleus::Block*,
                                       nucleus::Handle&);
      static elle::Status       Detach(nucleus::Handle&);
      static elle::Status       Load(nucleus::Handle&);
      static elle::Status       Unload(nucleus::Handle&);

      static elle::Status       Exist(const nucleus::Placement&);
      static elle::Status       Exist(const nucleus::Address&);
      static elle::Status       Add(const nucleus::Placement&,
                                    Pod*);
      static elle::Status       Add(const nucleus::Address&,
                                    Pod*);
      static elle::Status       Retrieve(const nucleus::Placement&,
                                         Pod*&);
      static elle::Status       Retrieve(const nucleus::Address&,
                                         Pod*&);
      static elle::Status       Remove(const nucleus::Placement&);
      static elle::Status       Retrieve(const nucleus::Address&);

      //
      // static attributes
      //
      static P::Container       Placements;
      static A::Container       Addresses;
    };

  }
}

#endif
