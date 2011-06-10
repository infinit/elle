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
// updated       julien quintard   [wed jun  1 11:42:05 2011]
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
    /// default constructor.
    ///
    Agent::Agent():
      state(Agent::StateUnauthenticated),
      channel(NULL)
    {
    }

    ///
    /// destructor.
    ///
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
    /// this method creates an remote agent by setting the communication
    /// channel.
    ///
    elle::Status	Agent::Create(const elle::PublicKey&	K,
				      elle::Channel*		channel)
    {
      elle::Callback<>				timeout(&Agent::Timeout, this);
      elle::Callback<const elle::String>	error(&Agent::Error, this);

      enter();

      // set the public key.
      this->K = K;

      // set the attributes.
      this->channel = channel;

      // create the timer.
      if (this->timer.Create(elle::Timer::ModeSingle,
			     timeout) == elle::StatusError)
	escape("unable to create the timer");

      // register the error callback to the deletion.
      if (this->channel->Monitor(error) == elle::StatusError)
	escape("unable to monitor the callback");

      // start the timer.
      if (this->timer.Start(Agent::Expiration) == elle::StatusError)
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

      // set the state.
      this->state = Agent::StateAuthenticated;

      // stop the timer.
      if (this->timer.Stop() == elle::StatusError)
	escape("unable to stop the timer");

      leave();
    }

    ///
    /// this method destroyes the agent.
    ///
    elle::Status	Agent::Destroy()
    {
      enter();

      // stop the timer, just in case.
      if (this->timer.Stop() == elle::StatusError)
	escape("unable to stop the timer");

      // withdraw the control management.
      if (this->channel->Withdraw() == elle::StatusError)
	escape("unable to withdraw the socket control");

      leave();
    }

    ///
    /// this method decrypts a code.
    ///
    elle::Status	Agent::Decrypt(const elle::Code&	code,
				       elle::Clear&		clear) const
    {
      enter();
      /* XXX
      // check if the user is authenticated.
      if (this->state != Agent::StateAuthenticated)
	escape("the agent seems not to have been authenticated");

      // send a request to the agent.
      if (this->channel->Call(
	    elle::Inputs<agent::TagDecrypt>(code),
	    elle::Outputs<agent::TagDecrypted>(clear)) ==
	  elle::StatusError)
	escape("unable to call the agent for decrypting a code");
      */
      leave();
    }

    ///
    /// this method signs a plain text.
    ///
    elle::Status	Agent::Sign(const elle::Plain&		plain,
				    elle::Signature&		signature)
      const
    {
      enter();
      /* XXX
      // check if the user is authenticated.
      if (this->state != Agent::StateAuthenticated)
	escape("the agent seems not to have been authenticated");

      // send a request to the agent.
      if (this->channel->Call(
	    elle::Inputs<agent::TagSign>(plain),
	    elle::Outputs<agent::TagSigned>(signature)) ==
	  elle::StatusError)
	escape("unable to call the agent for performing a signature");
      */
      leave();
    }

    ///
    /// this method encrypts the given data with the private key.
    ///
    elle::Status	Agent::Encrypt(const elle::Plain&	plain,
				       elle::Code&		code) const
    {
      enter();

      // XXX
      escape("NIY");

      leave();
    }

    ///
    /// this method derives a public key according to (i) its complementary
    /// private key and (ii) the seed used for rotating this key pair.
    ///
    elle::Status	Agent::Derive(const elle::Seed&		seed,
				      elle::PublicKey&		K) const
    {
      enter();

      // XXX
      escape("NIY");

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
      if (Client::Retrieve(this->channel, client) != elle::StatusTrue)
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
    /// this method dumps the agent.
    ///
    elle::Status	Agent::Dump(const elle::Natural32	margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Agent]" << std::endl;

      // dump the state.
      std::cout << alignment << elle::Dumpable::Shift << "[State] "
		<< this->state << std::endl;

      // dump the channel.
      if (this->channel->Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the channel");

      // dump the public key.
      if (this->K.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the public key");

      leave();
    }

  }
}
