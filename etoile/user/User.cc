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
// updated       julien quintard   [fri mar 12 07:16:32 2010]
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

    Client*				client;

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this container holds the clients.
    ///
    User::Individual::Container		User::Clients;

    ///
    /// this container holds the sockets in use.
    ///
    /// note that such a socket could very well be linked to no client
    /// until an agent gets authenticated.
    ///
    User::Access::Container		User::Sockets;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes the user module.
    ///
    Status		User::Initialize()
    {
      enter();

      leave();
    }

    ///
    /// this method cleans the user module by destroying all the clients
    /// and sockets.
    ///
    Status		User::Clean()
    {
      enter();

      /// XXX \todo clean the clients.

      /// XXX \todo clean the sockets (meaning the unathenticated sockets)

      leave();
    }

    ///
    /// this method adds a socket to the container.
    ///
    Status		User::Record(Socket*			socket)
    {
      std::pair<User::Access::Iterator, Boolean>	result;

      enter();

      // check if the socket already exists.
      if (User::Sockets.find(socket) != User::Sockets.end())
	escape("it seems that this socket is already registered");

      // simply add the socket to the sockets.
      result = User::Sockets.insert(User::Access::Value(socket,
							(Client*)NULL));

      // check if the insertion was successful.
      if (result.second == false)
	escape("unable to insert the socket in the container");

      leave();
    }

    ///
    /// this method registers an authenticated client.
    ///
    Status		User::Register(Client*			client)
    {
      User::Individual::Scoutor	scoutor;

      enter();

      // look for an existing client.
      for (scoutor = User::Clients.begin();
	   scoutor != User::Clients.end();
	   scoutor++)
	if (*scoutor == client)
	  escape("unable to register an alreay registered client");

      // add the client to the container.
      User::Clients.push_front(client);

      leave();
    }

    ///
    /// this method returns the client the given socket is associated with.
    ///
    Status		User::Locate(Socket*			socket,
				     Client*&			client)
    {
      User::Access::Scoutor	scoutor;

      enter();

      // look for the socket.
      scoutor = User::Sockets.find(socket);

      // check the scoutor.
      if (scoutor == User::Sockets.end())
	escape("unable to locate this socket");

      // return the associated client.
      client = scoutor->second;

      leave();
    }

    ///
    /// this method assigns the current client.
    ///
    Status		User::Assign(Client*			c)
    {
      enter();

      // set the current client.
      client = c;

      leave();
    }

  }
}
