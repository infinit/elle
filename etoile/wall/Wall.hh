//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/wall/Wall.hh
//
// created       julien quintard   [fri aug 14 12:56:07 2009]
// updated       julien quintard   [sun feb  7 12:06:07 2010]
//

#ifndef ETOILE_WALL_WALL_HH
#define ETOILE_WALL_WALL_HH

//
// ---------- includes --------------------------------------------------------
//

// XXX
#include <etoile/wall/Types.hh>
#include <etoile/wall/Object.hh>
#include <etoile/wall/Directory.hh>
// XXX

namespace etoile
{
  namespace wall
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Wall
    {
    public:
      //
      // types
      //
      typedef std::map<const String, Callback*>	Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // static methods
      //
      Status		Initialize();
      Status		Clean();

      //
      // static attributes
      //
      static Container	Interface;
    };

  }
}

#endif
