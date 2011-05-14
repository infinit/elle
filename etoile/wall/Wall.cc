//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/wall/Wall.cc
//
// created       julien quintard   [fri aug 14 12:57:57 2009]
// updated       julien quintard   [thu may 12 11:00:11 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/wall/Wall.hh>

#include <etoile/configuration/Configuration.hh>

#include <etoile/user/User.hh>

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
    const elle::String&		Wall::Line =
      configuration::Configuration::Wall::Line;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the wall by setting up the interface callbacks.
    ///
    elle::Status	Wall::Initialize()
    {
      elle::Callback<elle::Door*>		connection(&Wall::Connection);
      elle::Callback<const elle::Report>	error(&Wall::Error);

      enter();

      /// XXX \todo le faire proprement en quittant, tout nettoyer, notamment
      ///     en catchant les signaux (UNIX/QT) et via unlink dans Bridge.
      ::unlink("/tmp/etoile");

      // initialize the interface.
      if (Interface::Initialize() == elle::StatusError)
	escape("unable to initialize the interface");

      // register the error callback in order to dump the error occuring
      // in the agents for instance.
      if (elle::Network::Register<elle::TagError>(error) == elle::StatusError)
	escape("unable to register the error message");

      // listen for incoming connection.
      if (elle::Lane::Listen(Wall::Line, connection) == elle::StatusError)
	escape("unable to listen for bridge connections");

      leave();
    }

    ///
    /// this method cleans the callbacks.
    ///
    elle::Status	Wall::Clean()
    {
      enter();

      // clean the interface.
      if (Interface::Clean() == elle::StatusError)
	escape("unable to clean the interface");

      leave();
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this callback is triggered whenever an agent reports an error
    /// as a consequence of a request received from Etoile.
    ///
    /// note however that this should never occur!
    ///
    elle::Status	Wall::Error(const elle::Report&		report)
    {
      enter();

      printf("[XXX] Wall::Error\n");

      // XXX only dump if agent
      // XXX dump the agent object

      // dump the report.
      report.Dump();

      leave();
    }

    ///
    /// this method identifies the client by storing its identity as
    /// a guest before going on to the next step: authentication through
    /// public key challenge.
    ///
    elle::Status	Wall::Identify(const elle::PublicKey&	K)
    {
      elle::Session*	session;
      elle::Code	code;
      user::Guest*	guest;
      user::Agent*	agent;
      user::Client*	client;
      user::Client*	c;

      enter(instance(agent),
	    instance(client));

      printf("[XXX] Wall::Identify()\n");

      // retrieve the session.
      if (elle::Session::Instance(session) == elle::StatusError)
	escape("unable to retrieve the instance of the session");

      // retrieve the guest.
      if (user::Guest::Retrieve(static_cast<elle::Channel*>(session->socket),
				guest) == elle::StatusError)
	escape("unable to locate the sender's guest record");

      // check if this socket is already associated with a client. if so
      // that would mean that this operation is not authorised.
      if (user::Client::Retrieve(guest->channel, client) == elle::StatusTrue)
	escape("this socket seems to already be associated with a client");

      // check if this public key is already used. if so, that would mean
      // that an agent is already connected and handling this user.
      if (user::Client::Retrieve(K, client) == elle::StatusTrue)
	escape("this public key seems to already be handled by an agent");

      // detach the channel from the guest so that it does not get lost.
      if (guest->Detach() == elle::StatusError)
	escape("unable to detach the channel from the guest");

      // allocate a new agent.
      agent = new user::Agent;

      // create the agent.
      if (agent->Create(K, guest->channel) == elle::StatusError)
	escape("unable to create the agent");

      // allocate a new client.
      client = new user::Client;

      // create the client.
      if (client->Create() == elle::StatusError)
	escape("unable to create the client");

      // record the agent.
      if (client->Record(agent) == elle::StatusError)
	escape("unable to record the agent");

      // stop tracking the agent;
      waive(agent);

      // register the client.
      if (user::Client::Add(client) == elle::StatusError)
	escape("unable to add the client");

      // assign the client to the variable in order not to lose track
      // of it.
      c = client;

      // stop tracking the client.
      waive(client);

      // destroy the guest.
      if (user::Guest::Remove(guest) == elle::StatusError)
	escape("unable to destroy the guest");

      // encrypts the phrase with the user's public key.
      if (c->agent->K.Encrypt(c->phrase,
			      code) == elle::StatusError)
	escape("unable to encrypt the phrase");

      // send the challenge to the agent.
      if (c->agent->channel->Reply(
	    elle::Inputs<TagWallChallenge>(code)) == elle::StatusError)
	escape("unable to send the challenge to the agent");

      leave();
    }

    ///
    /// this method receives the digest of the challenge, computes the
    /// hash of the randomly generated challenge and compares it with
    /// the received digest.
    ///
    elle::Status	Wall::Authenticate(const elle::Digest&	digest)
    {
      user::User*	user;
      elle::Digest	d;

      enter();

      printf("[XXX] Wall::Authenticate()\n");

      // load the current user.
      if (user::User::Instance(user) == elle::StatusError)
	escape("unable to load the user");

      // check if the user is an agent.
      if (user->type != user::User::TypeAgent)
	escape("non-agents cannot authenticate");

      // check if the agent is not already authenticated.
      if (user->agent->state == user::Agent::StateAuthenticated)
	escape("this agent seems to be authenticated already");

      // compute the phrase digest.
      if (elle::OneWay::Hash(user->client->phrase, d) == elle::StatusError)
	escape("unable to hash the phrase");

      // compare the received digest with the phrase's one.
      if (digest != d)
	escape("the digest does not correspond to the encrypted phrase");

      // set the agent as authenticated.
      if (user->agent->Authenticate() == elle::StatusError)
	escape("unable to set the agent as authenticated");

      // acknowledge the authentication.
      if (user->agent->channel->Reply(elle::Inputs<TagOk>()) ==
	  elle::StatusError)
	escape("unable to acknowledge the authentication");

      leave();
    }

    ///
    /// this method receives the phrase originally used as a challenge
    /// for the agent. if it matches the agent phrase, the sender is
    /// considered as a valid application and will therefore be allowed
    /// to issue requests on behalf of the agent.
    ///
    elle::Status	Wall::Connect(const elle::String&	phrase,
				      const nucleus::Network&	network)
    {
      elle::Session*		session;
      user::Application*	application;
      user::Client*		client;
      user::Guest*		guest;
      user::User*		user;
      user::Application*	a;

      enter(instance(application));

      printf("[XXX] Wall::Connect()\n");

      // retrieve the session.
      if (elle::Session::Instance(session) == elle::StatusError)
	escape("unable to retrieve the instance of the session");

      // retrieve the guest.
      if (user::Guest::Retrieve(static_cast<elle::Channel*>(session->socket),
				guest) == elle::StatusError)
	escape("unable to locate the sender's guest record");

      // check if this socket is already associated with a client. if so
      // that would mean that this operation is not authorised.
      if (user::Client::Retrieve(guest->channel, client) == elle::StatusTrue)
	escape("this socket seems to already be associated with a client");

      // locate the client, that makes use of the given phrase.
      if (user::Client::Retrieve(phrase, client) != elle::StatusTrue)
	escape("unable to locate the agent making use of the given phrase");

      // check if the agent is authenticated.
      if (client->agent->state != user::Agent::StateAuthenticated)
	escape("this agent seems not to be authenticated");

      // detach the channel from the guest so that it does not get lost.
      if (guest->Detach() == elle::StatusError)
	escape("unable to detach the channel from the guest");

      // allocate a new application.
      application = new user::Application;

      // create a new application.
      if (application->Create(network,
			      guest->channel) == elle::StatusError)
	escape("unable to create the application");

      // register the application.
      if (client->Add(application) == elle::StatusError)
	escape("unable to add the application");

      // keep an eye on the application by copying its reference to
      // the user variable
      a = application;

      // waive the tracking.
      waive(application);

      // destroy the guest.
      if (user::Guest::Remove(guest) == elle::StatusError)
	escape("unable to destroy the guest");

      // acknowledge the connection.
      if (a->channel->Reply(elle::Inputs<TagOk>()) == elle::StatusError)
	escape("unable to acknowledge the connection");

      leave();
    }

    ///
    /// this callback is triggered whenever a connection is made to etoile
    /// through the wall.
    ///
    elle::Status	Wall::Connection(elle::Door*&		door)
    {
      user::Guest*	guest;

      enter(instance(guest));

      // allocate a new guest.
      guest = new user::Guest;

      // create the guest.
      if (guest->Create(door) == elle::StatusError)
	escape("unable to create the guest");

      // record the guest.
      if (user::Guest::Add(guest) == elle::StatusError)
	escape("unable to add the new guest");

      // stop tracking the guest.
      waive(guest);

      leave();
    }

  }
}
