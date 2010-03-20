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
// updated       julien quintard   [fri mar 19 21:40:30 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/user/Client.hh>

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
    Status		Client::Create()
    {
      char*		hexadecimal;
      Large		number;

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
    Status		Client::Destroy()
    {
      Client::A::Iterator	iterator;

      enter();

      // destroy the mapping for the agent.
      if (Map::Remove(this->agent->link) == StatusError)
	escape("unable to remove the agent mapping");

      // destroy the agent.
      if (this->agent->Destroy() == StatusError)
	escape("unable to destroy the agent");

      // destroy every application.
      for (iterator = this->applications.begin();
	   iterator != this->applications.end();
	   iterator++)
	{
	  // remove the mapping.
	  if (Map::Remove((*iterator)->link) == StatusError)
	    escape("unable to remove the application mapping");

	  // destroy the application
	  if ((*iterator)->Destroy() == StatusError)
	    escape("unable to destroy the application");
	}

      leave();
    }

    ///
    /// this method records the initial agent.
    ///
    Status		Client::Record(Agent*			agent)
    {
      enter();

      // set the agent.
      this->agent = agent;

      // add a mapping between the agent's link and the client.
      if (Map::Add(this->agent->link, this) == StatusError)
	escape("unable to add a mapping between the agent and the client");

      leave();
    }

    ///
    /// this method adds an application.
    ///
    Status		Client::Add(Application*		application)
    {
      enter();

      // add the application.
      this->applications.push_front(application);

      // add a mapping between the application's link and the client.
      if (Map::Add(application->link, this) == StatusError)
	escape("unable to add a mapping between the agent and the client");

      leave();
    }

    ///
    /// this method tries to locate an application.
    ///
    Status		Client::Locate(Application*		application,
				       A::Iterator&		iterator)
    {
      enter();

      // look for the link.
      for (iterator = this->applications.begin();
	   iterator != this->applications.end();
	   iterator++)
	if (application = (*iterator))
	  true();

      false();
    }

    ///
    /// this method adds an application.
    ///
    Status		Client::Remove(Application*		application)
    {
      Client::A::Iterator	iterator;

      enter();

      // locate the application.
      if (this->Locate(application, iterator) != StatusTrue)
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
    Status		Client::Dump(const Natural32		margin) const
    {
      String			alignment(margin, ' ');
      String			shift(2, ' ');
      Client::A::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Client] " << std::hex << this << std::endl;

      // dump the phrase
      std::cout << alignment << shift << "[Phrase] "
		<< this->phrase << std::endl;

      // dump the agent.
      if (this->agent->Dump(margin + 2) == StatusError)
	escape("unable to dump the agent");

      // dump each application.
      for (scoutor = this->applications.begin();
	   scoutor != this->applications.end();
	   scoutor++)
	{
	  // dump the application.
	  if ((*scoutor)->Dump(margin + 2) == StatusError)
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
    Status		Client::Initialize()
    {
      enter();

      // nothing to do

      leave();
    }

    ///
    /// this method cleans the client container.
    ///
    Status		Client::Clean()
    {
      enter();

      // remove every remaining client.
      while (Client::Clients.empty() == false)
	Client::Remove(Client::Clients.front());

      leave();
    }

    ///
    /// this method adds a client.
    ///
    Status		Client::Add(Client*			client)
    {
      enter();

      // add the client to the list.
      Client::Clients.push_front(client);

      leave();
    }

    ///
    /// this method tries to locate the given client.
    ///
    Status		Client::Locate(Client*			client,
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
    Status		Client::Remove(Client*			client)
    {
      Client::C::Iterator	iterator;

      enter();

      // try to locate the client.
      if (Client::Locate(client, iterator) != StatusTrue)
	escape("unable to locate the client to remove");

      // erase the client from the list.
      Client::Clients.erase(iterator);

      // destroy the client.
      if (client->Destroy() == StatusError)
	escape("unable to destroy the client");

      // delete the object.
      delete client;

      leave();
    }

    ///
    /// this method dumps the registered clients.
    ///
    Status		Client::Show(const Natural32		margin)
    {
      Client::C::Scoutor	scoutor;
      String			alignment(margin, ' ');
      String			shift(2, ' ');

      enter();

      std::cout << alignment << "[Clients]" << std::endl;

      // for every client.
      for (scoutor = Client::Clients.begin();
	   scoutor != Client::Clients.end();
	   scoutor++)
	{
	  // dump the client.
	  if ((*scoutor)->Dump(margin + 2) == StatusError)
	    escape("unable to dump the client");
	}

      leave();
    }

  }
}
