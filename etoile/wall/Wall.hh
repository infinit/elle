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
// updated       julien quintard   [fri mar  5 11:14:26 2010]
//

#ifndef ETOILE_WALL_WALL_HH
#define ETOILE_WALL_WALL_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/configuration/Configuration.hh>
#include <etoile/user/User.hh>

// XXX
#include <etoile/wall/Interface.hh>
#include <etoile/wall/Types.hh>
#include <etoile/wall/User.hh>
#include <etoile/wall/Object.hh>
#include <etoile/wall/Directory.hh>
// XXX

namespace etoile
{
  using namespace configuration;
  using namespace user;

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
      // constants
      //
      static const String&		Line;

      //
      // types
      //
      typedef std::list<Door*>		Container;
      typedef Container::iterator	Iterator;
      typedef Container::const_iterator	Scoutor;

      //
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();

      //
      // callbacks
      //
      static Status	Connection(Door*&);

      //
      // static attributes
      //
      static Container	Guests;
    };

  }
}

#endif
