//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/user/Agent.cc
//
// created       julien quintard   [thu mar 11 17:01:29 2010]
// updated       julien quintard   [thu apr 28 16:31:30 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/user/Agent.hh>
#include <etoile/user/Client.hh>
#include <etoile/user/User.hh>

namespace etoile
{
  namespace user
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this variable defines the time frame within which an agent should
    /// authentify.
    ///
    /// this value is in milli-seconds.
    ///
    const elle::Natural32	Agent::Expiration = 2000;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default remote constructor.
    ///
    Agent::Remote::Remote():
      state(Agent::StateUnauthenticated),
      channel(NULL)
    {
    }

    ///
    /// the remote destructor.
    ///
    Agent::Remote::~Remote()
    {
      // delete the channel, if present.
      if (this->channel != NULL)
	delete this->channel;
    }

    ///
    /// default constructor.
    ///
    Agent::Agent():
      type(Agent::TypeUnknown)
    {
    }

    ///
    /// destructor.
    ///
    Agent::~Agent()
    {
      // delete depending on the type.
      switch (this->type)
	{
	case Agent::TypeLocal:
	  {
	    // delete the local class.
	    if (this->local != NULL)
	      delete this->local;

	    break;
	  }
	case Agent::TypeRemote:
	  {
	    // delete the remote class.
	    if (this->remote != NULL)
	      delete this->remote;

	    break;
	  }
	}
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates an local agent by installing the agent's
    /// key pair.
    ///
    elle::Status	Agent::Create(const elle::KeyPair&	pair)
    {
      enter();

      // set the type.
      this->type = Agent::TypeLocal;

      // set the public key.
      this->K = pair.K;

      // allocate a new local.
      this->local = new Agent::Local;

      // set the private key.
      this->local->k = pair.k;

      leave();
    }

    ///
    /// this method creates an remote agent by setting the communication
    /// channel.
    ///
    elle::Status	Agent::Create(const elle::PublicKey&	K,
				      elle::Channel*		channel)
    {
      elle::Callback<>				timeout(&Agent::Timeout, this);
      elle::Callback<const elle::String>	error(&Agent::Error, this);

      enter();

      // set the type.
      this->type = Agent::TypeRemote;

      // set the public key.
      this->K = K;

      // allocate the remote.
      this->remote = new Agent::Remote;

      // set the attributes.
      this->remote->channel = channel;

      // create the timer.
      if (this->remote->timer.Create(elle::Timer::ModeSingle,
				     timeout) == elle::StatusError)
	escape("unable to create the timer");

      // register the error callback to the deletion.
      if (this->remote->channel->Monitor(error) == elle::StatusError)
	escape("unable to monitor the callback");

      // start the timer.
      if (this->remote->timer.Start(Agent::Expiration) == elle::StatusError)
	escape("unable to start the timer");

      leave();
    }

    ///
    /// this method marks the agent as authenticated by stopping the timer
    /// and so on.
    ///
    elle::Status	Agent::Authenticate()
    {
      enter();

      // check the type.
      if (this->type != Agent::TypeRemote)
	escape("unable to authenticate a local agent");

      // set the state.
      this->remote->state = Agent::StateAuthenticated;

      // stop the timer.
      if (this->remote->timer.Stop() == elle::StatusError)
	escape("unable to stop the timer");

      leave();
    }

    ///
    /// this method destroyes the agent.
    ///
    elle::Status	Agent::Destroy()
    {
      enter();

      // destroy depending on the type.
      switch (this->type)
	{
	case Agent::TypeLocal:
	  {
	    // nothing to do.
	    break;
	  }
	case Agent::TypeRemote:
	  {
	    // stop the timer, just in case.
	    if (this->remote->timer.Stop() == elle::StatusError)
	      escape("unable to stop the timer");

	    // withdraw the control management.
	    if (this->remote->channel->Withdraw() == elle::StatusError)
	      escape("unable to withdraw the socket control");

	    break;
	  }
	}

      leave();
    }

    ///
    /// this method decrypts a code by requesting the agent process
    /// to perform the cryptographic operation.
    ///
    elle::Status	Agent::Decrypt(const elle::Code&	code,
				       elle::Clear&		clear) const
    {
      enter();

      // perform the decryption depending on the agent's type.
      switch (this->type)
	{
	case Agent::TypeLocal:
	  {
	    // perform the decryption manually.
	    if (this->local->k.Decrypt(code, clear) == elle::StatusError)
	      escape("unable to decrypt the code");

	    break;
	  }
	case Agent::TypeRemote:
	  {
	    // check if the user is authenticated.
	    if (this->remote->state != Agent::StateAuthenticated)
	      escape("the agent seems not to have been authenticated");

	    // send a request to the agent.
	    if (this->remote->channel->Call(
		  elle::Inputs<agent::TagDecrypt>(code),
		  elle::Outputs<agent::TagDecrypted>(clear)) ==
		elle::StatusError)
	      escape("unable to call the agent for decrypting a code");

	    break;
	  }
	}

      leave();
    }

    ///
    /// this method signs a code by requesting the agent process
    /// to perform the cryptographic operation.
    ///
    elle::Status	Agent::Sign(const elle::Plain&		plain,
				    elle::Signature&		signature)
      const
    {
      enter();

      // perform the signing depending on the agent's type.
      switch (this->type)
	{
	case Agent::TypeLocal:
	  {
	    // perform the signing manually.
	    if (this->local->k.Sign(plain, signature) == elle::StatusError)
	      escape("unable to sign the plain");

	    break;
	  }
	case Agent::TypeRemote:
	  {
	    // check if the user is authenticated.
	    if (this->remote->state != Agent::StateAuthenticated)
	      escape("the agent seems not to have been authenticated");

	    // send a request to the agent.
	    if (this->remote->channel->Call(
		  elle::Inputs<agent::TagSign>(plain),
		  elle::Outputs<agent::TagSigned>(signature)) ==
		elle::StatusError)
	      escape("unable to call the agent for performing a signature");

	    break;
	  }
	}

      leave();
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this callbacks is triggered if the time frame expires.
    ///
    /// the call is just forwarded to the Error() callback.
    ///
    elle::Status	Agent::Timeout()
    {
      Client*		client;

      enter();

      // forward the call.
      if (this->Error("the agent has timed out") == elle::StatusError)
	escape("an error occured in the Error() callback\n");

      leave();
    }

    ///
    /// this callbacks is triggered if an error occurs on the channel.
    ///
    elle::Status	Agent::Error(const elle::String&	error)
    {
      Client::A::Iterator	iterator;
      elle::Report		report;
      Client*			client;

      enter();

      // retrieve the client related to this agent's channel.
      if (Client::Retrieve(this->remote->channel, client) != elle::StatusTrue)
	escape("unable to retrieve the client associated with the agent");

      // record an error message.
      report.Record(elle::Report::TypeError,
		    "_",
		    __DATE__ " "  __TIME__,
		    "the agent has been disconnected");

      // go through the client's applications.
      for (iterator = client->applications.begin();
	   iterator != client->applications.end();
	   iterator++)
	{
	  Application*		application = *iterator;

	  // send an error message to the application without caring
	  // if it succeeds as we just want to inform everyone before
	  // shutting every application related to this agent down.
	  application->channel->Send(elle::Inputs<elle::TagError>(report));
	}

      // remove the whole client since the agent timed-out.
      if (Client::Remove(client) == elle::StatusError)
	escape("unable to remove the client");

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps a local agent.
    ///
    elle::Status	Agent::Local::Dump(const elle::Natural32 margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Local]" << std::endl;

      // dump the private key.
      if (this->k.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the private key");

      leave();
    }

    ///
    /// this method dumps a remote agent.
    ///
    elle::Status	Agent::Remote::Dump(const elle::Natural32 margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Remote]" << std::endl;

      // dump the state.
      std::cout << alignment << elle::Dumpable::Shift << "[State] "
		<< this->state << std::endl;

      // dump the channel.
      if (this->channel->Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the channel");

      leave();
    }

    ///
    /// this method dumps the agent.
    ///
    elle::Status	Agent::Dump(const elle::Natural32	margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Agent] " << std::hex << this << std::endl;

      // dump the public key.
      if (this->K.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the public key");

      // dump the rest depending on the agent's type.
      switch (this->type)
	{
	case Agent::TypeLocal:
	  {
	    // dump the local agent.
	    if (this->local->Dump(margin + 2) == elle::StatusError)
	      escape("unable to dump the local agent");

	    break;
	  }
	case Agent::TypeRemote:
	  {
	    // dump the remote agent.
	    if (this->remote->Dump(margin + 2) == elle::StatusError)
	      escape("unable to dump the remote agent");

	    break;
	  }
	}

      leave();
    }

  }
}
