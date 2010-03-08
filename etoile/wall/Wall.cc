//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/wall/Wall.cc
//
// created       julien quintard   [fri aug 14 12:57:57 2009]
// updated       julien quintard   [fri mar  5 11:15:26 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/wall/Wall.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this defines the name of the door which will be used by clients.
    ///
    const String&		Wall::Line = Configuration::Wall::Line;

    ///
    /// this variable contains the unauthenticated connections.
    ///
    Wall::Container		Wall::Guests;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes the wall by setting up the interface callbacks.
    ///
    Status		Wall::Initialize()
    {
      static Function<Door*>	callback(&Wall::Connection);

      enter();

      /// XXX \todo le faire proprement en quittant, tout nettoyer, notamment
      ///     en catchant les signaux (UNIX/QT) et via unlink dans Bridge.
      ::unlink("/tmp/etoile");

      // initialize the interface.
      if (Interface::Initialize() == StatusError)
	escape("unable to initialize the interface");

      // listen for incoming connection.
      if (Bridge::Listen(Wall::Line, callback) == StatusError)
	escape("unable to listen for bridge connections");

      leave();
    }

    ///
    /// this method cleans the callbacks.
    ///
    Status		Wall::Clean()
    {
      enter();

      // clean the interface.
      if (Interface::Clean() == StatusError)
	escape("unable to clean the interface");

      leave();
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this callback is triggered whenever a connection is made to etoile
    /// through the wall.
    ///
    Status		Wall::Connection(Door*&			door)
    {
      enter();

      printf("Wall::Connection\n");

      // simply record the new guest in the list of unauthenticated
      // connections.
      Wall::Guests.push_front(door);

      leave();
    }

  }
}
