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
// updated       julien quintard   [tue apr  6 19:00:44 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/user/Agent.hh>

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
    const Natural32		Agent::Expiration = 2000; // XXX -0

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Agent::Agent():
      state(Agent::StateUnauthenticated),
      channel(NULL)
    {
    }

    Agent::~Agent()
    {
      // delete the channel, if present.
      if (this->channel != NULL)
	delete this->channel;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates the initial agent.
    ///
    Status		Agent::Create(const PublicKey&		K,
				      Channel*			channel)
    {
      Callback<>		timeout(&Agent::Timeout, this);
      Callback<const String>	error(&Agent::Error, this);

      enter();

      // set the attributes.
      this->state = Agent::StateUnauthenticated;
      this->K = K;
      this->channel = channel;

      // create the timer.
      if (this->timer.Create(Timer::ModeSingle, timeout) == StatusError)
	escape("unable to create the timer");

      // register the error callback to the deletion.
      if (this->channel->Monitor(error) == StatusError)
	escape("unable to monitor the callback");

      // start the timer.
      if (this->timer.Start(Agent::Expiration) == StatusError)
	escape("unable to start the timer");

      leave();
    }

    ///
    /// this method marks the agent as authenticated by stopping the timer
    /// and so on.
    ///
    Status		Agent::Authenticate()
    {
      enter();

      // set the state.
      this->state = Agent::StateAuthenticated;

      // stop the timer.
      if (this->timer.Stop() == StatusError)
	escape("unable to stop the timer");

      leave();
    }

    ///
    /// this method destroyes the agent.
    ///
    Status		Agent::Destroy()
    {
      enter();

      // stop the timer, just in case.
      if (this->timer.Stop() == StatusError)
	escape("unable to stop the timer");

      // withdraw the control management.
      if (this->channel->Withdraw() == StatusError)
	escape("unable to withdraw the socket control");

      leave();
    }

    ///
    /// this method decrypts a code by requesting the agent process
    /// to perform the cryptographic operation.
    ///
    Status		Agent::Decrypt(const Code&		code,
				       Clear&			clear) const
    {
      enter();

      // check if the user is authenticated.
      if (this->state != Agent::StateAuthenticated)
	escape("the agent seems not to have been authenticated");

      // send a request to the agent.
      if (this->channel->Call(
            Inputs< ::agent::TagDecrypt >(code),
	    Outputs< ::agent::TagDecrypted >(clear)) == StatusError)
	escape("unable to call the agent for decrypting a code");

      leave();
    }

    ///
    /// this method signs a code by requesting the agent process
    /// to perform the cryptographic operation.
    ///
    Status		Agent::Sign(const Plain&		plain,
				    Signature&			signature)
      const
    {
      enter();

      // check if the user is authenticated.
      if (this->state != Agent::StateAuthenticated)
	escape("the agent seems not to have been authenticated");

      // send a request to the agent.
      if (this->channel->Call(
            Inputs< ::agent::TagSign >(plain),
	    Outputs< ::agent::TagSigned >(signature)) == StatusError)
	escape("unable to call the agent for performing a signature");

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
    Status		Agent::Timeout()
    {
      Client*		client;

      enter();

      // forward the call.
      if (this->Error("the agent has timed out") == StatusError)
	escape("an error occured in the Error() callback\n");

      leave();
    }

    ///
    /// this callbacks is triggered if an error occurs on the channel.
    ///
    Status		Agent::Error(const String&		error)
    {
      Client::A::Iterator	iterator;
      Report			report;
      Client*			client;

      enter();

      // retrieve the client related to this agent's channel.
      if (Client::Retrieve(this->channel, client) != StatusTrue)
	escape("unable to retrieve the client associated with the agent");

      // record an error message.
      report.Record(Report::TypeError,
		    "",
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
	  application->channel->Send(Inputs<TagError>(report));
	}

      // remove the whole client since the agent timed-out.
      if (Client::Remove(client) == StatusError)
	escape("unable to remove the client");

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the agent.
    ///
    Status		Agent::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Agent] " << std::hex << this << std::endl;

      // dump the state.
      std::cout << alignment << Dumpable::Shift << "[State] "
		<< this->state << std::endl;

      // dump the public key.
      if (this->K.Dump(margin + 2) == StatusError)
	escape("unable to dump the public key");

      // dump the channel.
      if (this->channel->Dump(margin + 2) == StatusError)
	escape("unable to dump the channel");

      leave();
    }

  }
}
