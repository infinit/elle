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
// updated       julien quintard   [sat mar 20 15:33:04 2010]
//

#ifndef ETOILE_WALL_WALL_HH
#define ETOILE_WALL_WALL_HH

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/idiom/Close.hh>
# include <elle/Elle.hh>
#include <etoile/idiom/Open.hh>

#include <etoile/configuration/Configuration.hh>
#include <etoile/user/User.hh>

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
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();

      static Status	Identify(const PublicKey&);
      static Status	Authenticate(const Digest&);
      static Status	Connect(const String&);

      //
      // callbacks
      //
      static Status	Connection(Door*&);
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

// XXX
#include <etoile/wall/Interface.hh>
#include <etoile/wall/Types.hh>
#include <etoile/wall/User.hh>
#include <etoile/wall/Object.hh>
#include <etoile/wall/Directory.hh>
#include <etoile/wall/Result.hh>
// XXX

#endif
