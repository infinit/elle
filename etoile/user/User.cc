//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/user/User.cc
//
// created       julien quintard   [thu mar  4 12:39:12 2010]
// updated       julien quintard   [thu mar 25 18:03:13 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/user/User.hh>

namespace etoile
{
  namespace user
  {

//
// ---------- globals ---------------------------------------------------------
//

    ///
    /// this variable holds the identity of the current user.
    ///
    User				user;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    User::User():
      client(NULL)
    {
    }

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the user module.
    ///
    Status		User::Initialize()
    {
      enter();

      // initialize the client.
      if (Client::Initialize() == StatusError)
	escape("unable to initialize the client");

      // initialize the map.
      if (Map::Initialize() == StatusError)
	escape("unable to initialize the map");

      // initialize the guest.
      if (Guest::Initialize() == StatusError)
	escape("unable to initialize the guest");

      leave();
    }

    ///
    /// this method cleans the user module by destroying all the clients
    /// and channels.
    ///
    Status		User::Clean()
    {
      enter();

      // clean the guest.
      if (Guest::Clean() == StatusError)
	escape("unable to clean the guest");

      // clean the map.
      if (Map::Clean() == StatusError)
	escape("unable to clean the map");

      // clean the client.
      if (Client::Clean() == StatusError)
	escape("unable to clean the client");

      leave();
    }

    ///
    /// this method assigns the given client.
    ///
    Status		User::Assign(Client*			client)
    {
      enter();

      // set the current client.
      user.client = client;

      leave();
    }

    ///
    /// this method assigns the current socket as the current client.
    ///
    Status		User::Assign()
    {
      Client*		client;

      enter();

      // check if the session's socket is a channel.
      if ((session->socket->type & Socket::TypeChannel) == 0)
	escape("the session's socket is not a channel");

      // retrieve the client.
      if (Map::Retrieve(static_cast<Channel*>(session->socket),
			client) == StatusError)
	escape("unablt to retrive the client from the session's socket");

      // assign this client.
      if (User::Assign(client) == StatusError)
	escape("unable to assign the session's client");

      leave();
    }

  }
}
