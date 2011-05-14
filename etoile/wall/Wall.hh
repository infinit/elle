//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/wall/Wall.hh
//
// created       julien quintard   [fri aug 14 12:56:07 2009]
// updated       julien quintard   [thu may 12 00:16:29 2011]
//

#ifndef ETOILE_WALL_WALL_HH
#define ETOILE_WALL_WALL_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/Manifest.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// the Wal method deal with identifying, authenticating and connecting
    /// clients so that once authenticated/connected, an application can
    /// issue requests.
    ///
    class Wall
    {
    public:
      //
      // constants
      //
      static const elle::String&		Line;

      //
      // static methods
      //
      static elle::Status	Initialize();
      static elle::Status	Clean();

      static elle::Status	Identify(const elle::PublicKey&);
      static elle::Status	Authenticate(const elle::Digest&);
      static elle::Status	Connect(const elle::String&,
					const nucleus::Network&);

      //
      // callbacks
      //
      static elle::Status	Error(const elle::Report&);
      static elle::Status	Connection(elle::Door*&);
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/wall/Access.hh>
#include <etoile/wall/Attributes.hh>
#include <etoile/wall/Directory.hh>
#include <etoile/wall/File.hh>
#include <etoile/wall/Interface.hh>
#include <etoile/wall/Link.hh>
#include <etoile/wall/Object.hh>
#include <etoile/wall/Status.hh>
#include <etoile/wall/User.hh>

#endif
