//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/user/Application.cc
//
// created       julien quintard   [thu mar 11 17:09:58 2010]
// updated       julien quintard   [thu apr 22 10:09:00 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/user/Application.hh>

namespace etoile
{
  namespace user
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Application::Application():
      channel(NULL)
    {
    }

    ///
    /// destructor.
    ///
    Application::~Application()
    {
      // release the channel.
      if (this->channel != NULL)
	delete this->channel;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates the channel.
    ///
    Status		Application::Create(Channel*		channel)
    {
      Callback<const String>	error(&Application::Error, this);

      enter();

      // set the attributes.
      this->channel = channel;

      // register the error callback to the deletion.
      if (this->channel->Monitor(error) == StatusError)
	escape("unable to monitor the callback");

      leave();
    }

    ///
    /// this method destroys the application.
    ///
    Status		Application::Destroy()
    {
      enter();

      // withdraw the control management.
      if (this->channel->Withdraw() == StatusError)
	escape("unable to withdraw the socket control");

      // XXX go through the contexts: delete them and delete the associated
      // fibers

      leave();
    }

    ///
    /// this method removes a recorded context.
    ///
    Status		Application::Remove(const
					      context::Identifier& identifier)
    {
      Application::Iterator	iterator;
      context::Context*		context;

      enter();

      // find the entry.
      if ((iterator = this->contexts.find(identifier)) == this->contexts.end())
	escape("unable to locate the context associated with the "
	       "given identifier");

      // retrieve the context.
      context = iterator->second;

      // erase the entry.
      this->contexts.erase(iterator);

      leave();
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this callbacks is triggered if an error occurs on the channel.
    ///
    Status		Application::Error(const String&)
    {
      Client*		client;

      enter();

      printf("[XXX] Application:Error()\n");

      // retrieve the client related to this application's channel.
      if (Client::Retrieve(this->channel, client) != StatusTrue)
	escape("unable to retrieve the client associated with the agent");

      // remove the application from the client.
      if (client->Remove(this) == StatusError)
	escape("unable to remove the application");

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the application.
    ///
    Status		Application::Dump(const Natural32	margin) const
    {
      String			alignment(margin, ' ');
      Application::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Application] "
		<< std::hex << this << std::endl;

      // dump the channel.
      if (this->channel->Dump(margin + 2) == StatusError)
	escape("unable to dump the channel");

      std::cout << alignment << Dumpable::Shift << "[Contexts] "
		<< std::dec << this->contexts.size() << std::endl;

      // dump the contexts' addresses.
      for (scoutor = this->contexts.begin();
	   scoutor != this->contexts.end();
	   scoutor++)
	{
	  std::cout << alignment << Dumpable::Shift << Dumpable::Shift
		    << "[" << scoutor->first << "] "
		    << scoutor->second << std::endl;
	}

      leave();
    }

  }
}
