//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/user/Client.cc
//
// created       julien quintard   [thu mar 11 16:21:11 2010]
// updated       julien quintard   [fri may  6 13:54:06 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/user/Client.hh>
#include <etoile/user/Map.hh>

namespace etoile
{
  namespace user
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this container holds the clients.
    ///
    Client::C::Container		Client::Clients;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Client::Client():
      agent(NULL)
    {
    }

    ///
    /// destructor,
    ///
    Client::~Client()
    {
      Client::A::Scoutor	scoutor;

      // release the agent.
      if (this->agent != NULL)
	delete this->agent;

      // go through the applications and delete them.
      for (scoutor = this->applications.begin();
	   scoutor != this->applications.end();
	   scoutor++)
	delete *scoutor;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates the client by generating a random phrase.
    ///
    elle::Status	Client::Create()
    {
      char*		hexadecimal;
      elle::Large	number;

      enter();

      // generate a large number.
      ::BN_init(&number);
      ::BN_rand(&number, 256, 0, 0);

      // retrieve the hexadecimal representation of the number.
      hexadecimal = ::BN_bn2hex(&number);

      // create the phrase.
      this->phrase.assign(hexadecimal);

      // free the hexadecimal representation.
      ::OPENSSL_free(hexadecimal);

      leave();
    }

    ///
    /// this method destroys the client.
    ///
    elle::Status	Client::Destroy()
    {
      Client::A::Iterator	iterator;

      enter();

      // destroy the mapping for the agent.
      if (Map::Remove(this->agent->channel) == elle::StatusError)
	escape("unable to remove the agent mapping");

      // destroy the agent.
      if (this->agent->Destroy() == elle::StatusError)
	escape("unable to destroy the agent");

      // destroy every application.
      for (iterator = this->applications.begin();
	   iterator != this->applications.end();
	   iterator++)
	{
	  // remove the mapping.
	  if (Map::Remove((*iterator)->channel) == elle::StatusError)
	    escape("unable to remove the application mapping");

	  // destroy the application
	  if ((*iterator)->Destroy() == elle::StatusError)
	    escape("unable to destroy the application");
	}

      leave();
    }

    ///
    /// this method records the initial agent.
    ///
    elle::Status	Client::Record(Agent*			agent)
    {
      enter();

      // set the agent.
      this->agent = agent;

      // add a mapping between the agent's channel and the client.
      if (Map::Add(this->agent->channel, this) == elle::StatusError)
	escape("unable to add a mapping between the agent and the client");

      // create the subject in order to makes manipulating the user
      // a lot easier in many contexts.
      if (this->subject.Create(agent->K) == elle::StatusError)
	escape("unable to create the subject");

      leave();
    }

    ///
    /// this method adds an application.
    ///
    elle::Status	Client::Add(Application*		application)
    {
      enter();

      // add the application.
      this->applications.push_front(application);

      // add a mapping between the application's channel and the client.
      if (Map::Add(application->channel, this) == elle::StatusError)
	escape("unable to add a mapping between the agent and the client");

      leave();
    }

    ///
    /// this method tries to locate an application.
    ///
    elle::Status	Client::Locate(Application*		application,
				       A::Iterator&		iterator)
    {
      enter();

      // look for the channel.
      for (iterator = this->applications.begin();
	   iterator != this->applications.end();
	   iterator++)
	if (application = (*iterator))
	  true();

      false();
    }

    ///
    /// this method returns the application associated with the given channel.
    ///
    elle::Status	Client::Retrieve(const elle::Channel*	channel,
					 Application*&		application)
    {
      Client::A::Scoutor	scoutor;

      enter();

      // look for the given application.
      for (scoutor = this->applications.begin();
	   scoutor != this->applications.end();
	   scoutor++)
	{
	  if ((*scoutor)->channel == channel)
	    {
	      // set the application pointer.
	      application = *scoutor;

	      leave();
	    }
	}

      escape("unable to locate the given channel among the applications");
    }

    ///
    /// this method adds an application.
    ///
    elle::Status	Client::Remove(Application*		application)
    {
      Client::A::Iterator	iterator;

      enter();

      // locate the application.
      if (this->Locate(application, iterator) != elle::StatusTrue)
	escape("unable to locate the given application");

      // remove the application.
      this->applications.erase(iterator);

      // delete the application object.
      delete application;

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps a client.
    ///
    elle::Status	Client::Dump(const elle::Natural32	margin) const
    {
      elle::String		alignment(margin, ' ');
      Client::A::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Client] " << std::hex << this << std::endl;

      // dump the phrase
      std::cout << alignment << elle::Dumpable::Shift << "[Phrase] "
		<< this->phrase << std::endl;

      // dump the agent.
      if (this->agent->Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the agent");

      // dump the applications.
      std::cout << alignment << elle::Dumpable::Shift << "[Applications]"
		<< std::endl;

      // dump each application.
      for (scoutor = this->applications.begin();
	   scoutor != this->applications.end();
	   scoutor++)
	{
	  // dump the application.
	  if ((*scoutor)->Dump(margin + 4) == elle::StatusError)
	    escape("unable to dump the application");
	}

      leave();
    }

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the client container.
    ///
    elle::Status	Client::Initialize()
    {
      enter();

      // nothing to do

      leave();
    }

    ///
    /// this method cleans the client container.
    ///
    elle::Status	Client::Clean()
    {
      enter();

      // remove every remaining client.
      while (Client::Clients.empty() == false)
	{
	  Client*	client = Client::Clients.front();

	  // remove the client.
	  if (Client::Remove(client) == elle::StatusError)
	    escape("unable to remove the client");
	}

      leave();
    }

    ///
    /// this method adds a client.
    ///
    elle::Status	Client::Add(Client*			client)
    {
      enter();

      // add the client to the list.
      Client::Clients.push_front(client);

      leave();
    }

    ///
    /// this method tries to locate the given client.
    ///
    elle::Status	Client::Locate(Client*			client,
				       C::Iterator&		iterator)
    {
      enter();

      // look for the given client.
      for (iterator = Client::Clients.begin();
	   iterator != Client::Clients.end();
	   iterator++)
	if (client == *iterator)
	  true();

      false();
    }

    ///
    /// this method removes a client.
    ///
    elle::Status	Client::Remove(Client*			client)
    {
      Client::C::Iterator	iterator;

      enter();

      // try to locate the client.
      if (Client::Locate(client, iterator) != elle::StatusTrue)
	escape("unable to locate the client to remove");

      // erase the client from the list.
      Client::Clients.erase(iterator);

      // destroy the client.
      if (client->Destroy() == elle::StatusError)
	escape("unable to destroy the client");

      // delete the object.
      delete client;

      leave();
    }

    ///
    /// this method locates a client from a channel.
    ///
    elle::Status	Client::Retrieve(const elle::Channel*	channel,
					 Client*&		client)
    {
      return (Map::Retrieve(channel, client));
    }

    ///
    /// this method retrieves a client from a public key.
    ///
    elle::Status	Client::Retrieve(const elle::PublicKey&	K,
					 Client*&		client)
    {
      Client::C::Scoutor	scoutor;

      enter();

      // for every client.
      for (scoutor = Client::Clients.begin();
	   scoutor != Client::Clients.end();
	   scoutor++)
	{
	  // set the client.
	  client = *scoutor;

	  // check if the agent is handling this public key.
	  if (client->agent->K == K)
	    true();
	}

      false();
    }

    ///
    /// this method retrieves a client from a phrase.
    ///
    elle::Status	Client::Retrieve(const elle::String&	phrase,
					 Client*&		client)
    {
      Client::C::Scoutor	scoutor;

      enter();

      // for every client.
      for (scoutor = Client::Clients.begin();
	   scoutor != Client::Clients.end();
	   scoutor++)
	{
	  // set the client.
	  client = *scoutor;

	  // check if this client uses the given phrase.
	  if (client->phrase == phrase)
	    true();
	}

      false();
    }

    ///
    /// this method dumps the registered clients.
    ///
    elle::Status	Client::Show(const elle::Natural32	margin)
    {
      Client::C::Scoutor	scoutor;
      elle::String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Clients]" << std::endl;

      // for every client.
      for (scoutor = Client::Clients.begin();
	   scoutor != Client::Clients.end();
	   scoutor++)
	{
	  // dump the client.
	  if ((*scoutor)->Dump(margin + 2) == elle::StatusError)
	    escape("unable to dump the client");
	}

      leave();
    }

  }
}
