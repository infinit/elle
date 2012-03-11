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

namespace etoile
{
  namespace nest
  {

//
// ---------- classes ---------------------------------------------------------
//

/* XXX
   recup block via placement
    - load classique de memoire
    - load via disque: placement represente le nom du fichier
    - load via adresse: le block recherche est peut etre deja dans le nest,
        et a deja peut etre ete modifie -> meme handle/_block
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
        typedef std::map<const nucleus::Placement,
                         nucleus::Handle*>              Container;
        typedef typename Container::iterator            Iterator;
        typedef typename Container::const_iterator      Scoutor;
      };

      struct A
      {
        typedef std::map<const nucleus::Address,
                         nucleus::Handle*>              Container;
        typedef typename Container::iterator            Iterator;
        typedef typename Container::const_iterator      Scoutor;
      };

      //
      // static methods
      //
      static elle::Status       Register(nucleus::Handle&);

      static elle::Status       Load(nucleus::Handle&);
      static elle::Status       Unload(nucleus::Handle&);

      //
      // static attributes
      //
      static P::Container       Placements;
      static A::Container       Addresses;
    };

  }
}

#endif
