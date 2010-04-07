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
// updated       julien quintard   [thu apr  1 02:55:05 2010]
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
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Application] "
		<< std::hex << this << std::endl;

      // dump the channel.
      if (this->channel->Dump(margin + 2) == StatusError)
	escape("unable to dump the channel");

      leave();
    }

  }
}
