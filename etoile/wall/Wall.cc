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
// updated       julien quintard   [sat mar 27 20:53:27 2010]
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

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the wall by setting up the interface callbacks.
    ///
    Status		Wall::Initialize()
    {
      Callback<Door*>	callback(&Wall::Connection);

      enter();

      /// XXX \todo le faire proprement en quittant, tout nettoyer, notamment
      ///     en catchant les signaux (UNIX/QT) et via unlink dans Bridge.
      ::unlink("/tmp/etoile");

      // initialize the interface.
      if (Interface::Initialize() == StatusError)
	escape("unable to initialize the interface");

      // listen for incoming connection.
      if (Lane::Listen(Wall::Line, callback) == StatusError)
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

    ///
    /// XXX
    ///
    Status		Wall::Identify(const PublicKey&		K)
    {
      Code		code;
      user::Guest*	guest;
      user::Agent*	agent;
      user::Client*	client;

      enter(instance(agent),
	    instance(client));

      printf("[XXX] Wall::Identify()\n");

      /// XXX \todo regarde si il y a deja un client sur cette socket,
      /// et verifier que l'agent n'est pas deja authentifie
      /// et surtout que cette clef publique n'a pas deja un agent!!!

      // retrieve the guest.
      if (user::Guest::Retrieve((Channel*)session->socket, guest) ==
	  StatusError)
	escape("unable to locate the sender's guest record");

      // allocate a new agent.
      agent = new user::Agent;

      // create the agent.
      if (agent->Create(K, guest->channel) == StatusError)
	escape("unable to create the agent");

      // allocate a new client.
      client = new user::Client;

      // create the client.
      if (client->Create() == StatusError)
	escape("unable to create the client");

      // record the agent.
      if (client->Record(agent) == StatusError)
	escape("unable to record the agent");

      // assign the client as being the current one.
      if (user::User::Assign(client) == StatusError)
	escape("unable to assign the current user");

      // stop tracking the agent;
      waive(agent);

      // register the client.
      if (user::Client::Add(client) == StatusError)
	escape("unable to add the client");

      // stop tracking the client.
      waive(client);

      // detach the channel from the guest so that it does not get lost.
      if (guest->Detach() == StatusError)
	escape("unable to detach the channel from the guest");

      // destroy the guest.
      if (user::Guest::Remove(guest) == StatusError)
	escape("unable to destroy the guest");

      // encrypts the phrase with the user's public key.
      if (user::user.client->agent->K.Encrypt(user::user.client->phrase,
					      code) == StatusError)
	escape("unable to encrypt the phrase");

      // send the challenge to the agent.
      if (user::user.client->agent->channel->Reply(
	    Inputs<TagWallChallenge>(code)) == StatusError)
	escape("unable to send the challenge to the agent");

      printf("[/XXX] Wall::Identify()\n");

      leave();
    }

    ///
    /// XXX
    ///
    Status		Wall::Authenticate(const Digest&	digest)
    {
      Digest		d;

      enter();

      printf("[XXX] Wall::Authenticate()\n");

      /// XXX \todo regarde si il y a deja un client sur cette socket,
      /// et verifier que l'agent n'est pas deja authentifie

      // load the current user.
      if (user::User::Assign() == StatusError)
	escape("unable to load the user");

      // compute the phrase digest.
      if (OneWay::Hash(user::user.client->phrase, d) == StatusError)
	escape("unable to compute the phrase's digest");

      // compare the received digest with the phrase's one.
      if (digest != d)
	abort(user::user.client->agent->channel,
	      "the digest does not correspond to the encrypted phrase");

      // set the agent as authenticated.
      if (user::user.client->agent->Authenticate() == StatusError)
	escape("unable to set the agent as authenticated");

      // acknowledge the authentication.
      acknowledge(user::user.client->agent->channel,
		  TagWallAuthenticated);

      printf("[/XXX] Wall::Authenticate()\n");

      leave();
    }

    ///
    /// XXX
    ///
    Status		Wall::Connect(const String&		phrase)
    {
      enter();

      printf("[XXX] Wall::Connect()\n");

      printf("[/XXX] Wall::Connect()\n");

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
      Guest*		guest;

      enter(instance(guest));

      // allocate a new guest.
      guest = new user::Guest;

      // create the guest.
      if (guest->Create(door) == StatusError)
	escape("unable to create the guest");

      // record the guest.
      if (user::Guest::Add(guest) == StatusError)
	escape("unable to add the new guest");

      // stop tracking the guest.
      waive(guest);

      leave();
    }

  }
}
