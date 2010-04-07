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
// updated       julien quintard   [thu apr  1 03:15:46 2010]
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
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this variable represents the current user.
    ///
    User*			User::Current = NULL;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    User::User():
      socket(NULL),
      client(NULL),
      type(User::TypeUnknown)
    {
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the user.
    ///
    Status		User::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[User] " << std::hex << this << std::endl;

      // dump the socket.
      std::cout << alignment << Dumpable::Shift << "[Socket] "
		<< std::hex << this->socket << std::endl;

      // dump the client.
      std::cout << alignment << Dumpable::Shift << "[Client] "
		<< std::hex << this->client << std::endl;

      // dump the type.
      std::cout << alignment << Dumpable::Shift << "[Type] "
		<< this->type << std::endl;

      // dump the content.
      switch (this->type)
	{
	case User::TypeAgent:
	  {
	    std::cout << alignment << Dumpable::Shift << "[Agent] "
		      << std::hex << this->agent << std::endl;

	    break;
	  }
	case User::TypeApplication:
	  {
	    std::cout << alignment << Dumpable::Shift << "[Application] "
		      << std::hex << this->application << std::endl;

	    break;
	  }
	}

      leave();
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

      // allocate the user.
      User::Current = new User;

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

      // release the user.
      delete User::Current;

      leave();
    }

    ///
    /// this method returns the current User instance depending on
    /// the session.
    ///
    Status		User::Instance(User*&			user)
    {
      enter();

      // if the currently loaded user does not corresponds to the
      // session, load it.
      if (User::Current->socket != session->socket)
	{
	  Client*	client;

	  // check if the session's socket is a channel.
	  if ((session->socket->type & Socket::TypeChannel) == 0)
	    escape("the session's socket is not a channel");

	  // retrieve the client.
	  if (Map::Retrieve(static_cast<Channel*>(session->socket),
			    client) == StatusError)
	    escape("unablt to retrive the client from the session's socket");

	  // create the user.
	  if (User::Current->Assign(client) == StatusError)
	    escape("unable to assign the user");
	}
      else
	{
	  // otherwise, the current user is already the right one.
	}

      // return the user.
      user = User::Current;

      leave();
    }

    ///
    /// this method creates a user based on the given client.
    ///
    Status		User::Assign(Client*			client)
    {
      enter();

      // set the client.
      User::Current->client = client;

      // assign the type and agent/application pointers.
      if (session->socket == User::Current->client->agent->channel)
	{
	  // set the type.
	  User::Current->type = User::TypeAgent;

	  // set the agent.
	  User::Current->agent = User::Current->client->agent;
	}
      else
	{
	  // set the type.
	  User::Current->type = User::TypeApplication;

	  // set the application.
	  if (User::Current->client->Retrieve(
                static_cast<Channel*>(session->socket),
		User::Current->application) == StatusError)
	    escape("unable to retrieve the application belonging "
		   "to the client");
	}

      // set the socket.
      User::Current->socket = session->socket;

      leave();
    }

  }
}
