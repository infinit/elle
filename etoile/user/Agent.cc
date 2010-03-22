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
// updated       julien quintard   [sun mar 21 22:08:54 2010]
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
    const Natural32		Agent::Expiration = 20000; // XXX -0

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Agent::Agent():
      state(Agent::StateUnauthenticated),
      link(NULL)
    {
    }

    Agent::~Agent()
    {
      // delete the link, if present.
      if (this->link != NULL)
	delete this->link;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates the initial agent.
    ///
    Status		Agent::Create(const PublicKey&		K,
				      Link*			link)
    {
      Method<>			timeout(this, &Agent::Timeout);
      Method<const String>	error(this, &Agent::Error);

      enter();

      // set the attributes.
      this->state = Agent::StateUnauthenticated;
      this->K = K;
      this->link = link;

      // create the timer.
      if (this->timer.Create(Timer::ModeSingle, timeout) == StatusError)
	escape("unable to create the timer");

      // register the error callback to the deletion.
      if (this->link->Monitor(error) == StatusError)
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
      if (this->link->Withdraw() == StatusError)
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

      printf("[XXX] Agent::Decrypt()\n");

      // send a request to the agent.
      /*
      if (this->link->Call(
            Inputs< ::agent::TagDecrypt >(code),
	    Outputs< ::agent::TagDecrypted >(clear)) == StatusError)
	escape("unable to call the agent for decrypting a code");
      */
      if (this->link->Send(Inputs< ::agent::TagDecrypt >(code)) == StatusError)
	escape("ERROR");

      printf("[XXX] /Agent::Decrypt()\n");

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

      printf("[XXX] Agent::Sign\n");

      // send a request to the agent.
      if (this->link->Call(
            Inputs< ::agent::TagSign >(plain),
	    Outputs< ::agent::TagSigned >(signature)) == StatusError)
	escape("unable to call the agent for performing a signature");

      printf("[XXX] /Agent::Sign\n");

      leave();
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this callbacks is triggered if the time frame expires.
    ///
    Status		Agent::Timeout()
    {
      Client*		client;

      enter();

      // retrieve the client related to this agent's link.
      if (Map::Retrieve(this->link, client) == StatusError)
	escape("unable to retrieve the client associated with the agent");

      // remove the whole client since the agent timed-out.
      if (Client::Remove(client) == StatusError)
	escape("unable to remove the client");

      printf("[XXX] Agent::Timeout()\n");

      leave();
    }

    ///
    /// this callbacks is triggered if an error occurs on the link.
    ///
    Status		Agent::Error(const String&)
    {
      Client*		client;

      enter();

      // retrieve the client related to this agent's link.
      if (Map::Retrieve(this->link, client) == StatusError)
	escape("unable to retrieve the client associated with the agent");

      // remove the whole client since the agent timed-out.
      if (Client::Remove(client) == StatusError)
	escape("unable to remove the client");

      printf("[XXX] Agent::Error()\n");

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
      String		shift(2, ' ');

      enter();

      std::cout << alignment << "[Agent]" << std::endl;

      // dump the state.
      std::cout << alignment << shift << "[State] "
		<< this->state << std::endl;

      // dump the public key.
      if (this->K.Dump(margin + 2) == StatusError)
	escape("unable to dump the public key");

      // dump the link.
      if (this->link->Dump(margin + 2) == StatusError)
	escape("unable to dump the link");

      leave();
    }

  }
}
