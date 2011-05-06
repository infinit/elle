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
// updated       julien quintard   [fri may  6 13:55:01 2011]
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
      session(NULL),
      client(NULL),
      type(User::TypeUnknown)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a user based on the given client.
    ///
    elle::Status	User::Create(Client*			client)
    {
      elle::Session*	session;

      enter();

      // retrieve the session.
      if (elle::Session::Instance(session) == elle::StatusError)
	escape("unable to retrieve the session");

      // set the client.
      this->client = client;

      // assign the type and agent/application pointers.
      if (session->socket == this->client->agent->channel)
	{
	  // set the type.
	  this->type = User::TypeAgent;

	  // set the agent.
	  this->agent = this->client->agent;
	}
      else
	{
	  // set the type.
	  this->type = User::TypeApplication;

	  // set the application.
	  if (this->client->Retrieve(
		static_cast<elle::Channel*>(session->socket),
		this->application) == elle::StatusError)
	    escape("unable to retrieve the application belonging "
		   "to the client");
	}

      // set the session;
      this->session = session;

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the user.
    ///
    elle::Status	User::Dump(const elle::Natural32	margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[User] " << std::hex << this << std::endl;

      // dump the session.
      if (this->session->Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the session");

      // dump the client.
      std::cout << alignment << elle::Dumpable::Shift << "[Client] "
		<< std::hex << this->client << std::endl;

      // dump the type.
      std::cout << alignment << elle::Dumpable::Shift << "[Type] "
		<< this->type << std::endl;

      // dump the content.
      switch (this->type)
	{
	case User::TypeAgent:
	  {
	    std::cout << alignment << elle::Dumpable::Shift << "[Agent] "
		      << std::hex << this->agent << std::endl;

	    break;
	  }
	case User::TypeApplication:
	  {
	    std::cout << alignment << elle::Dumpable::Shift << "[Application] "
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
    elle::Status	User::Initialize()
    {
      elle::Callback<const elle::Phase,
	             elle::Fiber*>		govern(&User::Govern);

      enter();

      // initialize the map.
      if (Map::Initialize() == elle::StatusError)
	escape("unable to initialize the map");

      // initialize the client.
      if (Client::Initialize() == elle::StatusError)
	escape("unable to initialize the client");

      // initialize the guest.
      if (Guest::Initialize() == elle::StatusError)
	escape("unable to initialize the guest");

      // register the callback to the fiber system.
      if (elle::Fiber::Register(govern) == elle::StatusError)
	escape("unable to register the govern callback");

      leave();
    }

    ///
    /// this method cleans the user module by destroying all the clients
    /// and channels.
    ///
    elle::Status	User::Clean()
    {
      enter();

      // clean the guest.
      if (Guest::Clean() == elle::StatusError)
	escape("unable to clean the guest");

      // clean the client.
      if (Client::Clean() == elle::StatusError)
	escape("unable to clean the client");

      // clean the map.
      if (Map::Clean() == elle::StatusError)
	escape("unable to clean the map");

      leave();
    }

    ///
    /// this method returns the current User instance depending on
    /// the session.
    ///
    elle::Status	User::Instance(User*&			user)
    {
      elle::Session*	session;

      enter();

      // retrieve the session.
      if (elle::Session::Instance(session) == elle::StatusError)
	escape("unable to retrieve the session");

      // if the currently loaded user does not corresponds to the
      // session, load it.
      if (User::Current->session != session)
	{
	  Client*	client;

	  // check if the session's socket is a channel.
	  if ((session->socket->type & elle::Socket::TypeChannel) == 0)
	    escape("the session's socket is not a channel");

	  // retrieve the client.
	  if (Map::Retrieve(static_cast<elle::Channel*>(session->socket),
			    client) == elle::StatusError)
	    escape("unablt to retrive the client from the session's socket");

	  // create the user.
	  if (User::Current->Create(client) == elle::StatusError)
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
    /// this method initializes, saves, restores and cleans the user
    /// associated with the current fiber.
    ///
    elle::Status	User::Govern(const elle::Phase&		phase,
				     elle::Fiber*&		fiber)
    {
      enter();

      // perform an operation depending on the phase.
      switch (phase)
	{
	case elle::PhaseInitialize:
	  {
	    // allocate a new user.
	    User::Current = new User;

	    break;
	  }
	case elle::PhaseSave:
	  {
	    // save the user in the fiber's environment.
	    if (fiber->environment->Store("user",
					  User::Current) == elle::StatusError)
	      escape("unable to store the user in the environment");

	    // set the pointer to NULL, for safety purposes.
	    User::Current = NULL;

	    break;
	  }
	case elle::PhaseRestore:
	  {
	    // restore the user from the fiber's environment.
	    if (fiber->environment->Load("user",
					 User::Current) == elle::StatusError)
	      escape("unable to load the user from the environment");

	    break;
	  }
	case elle::PhaseClean:
	  {
	    // delete the user.
	    delete User::Current;

	    // for safety purposes, reset the pointer to NULL.
	    User::Current = NULL;

	    break;
	  }
	}

      leave();
    }

  }
}
