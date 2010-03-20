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
// updated       julien quintard   [fri mar 19 21:57:29 2010]
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
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this variable defines the time frame within which an application should
    /// connect.
    ///
    /// this value is in milli-seconds.
    ///
    const Natural32		Application::Expiration = 2000;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Application::Application():
      link(NULL)
    {
    }

    ///
    /// destructor.
    ///
    Application::~Application()
    {
      // release the link.
      if (this->link != NULL)
	delete this->link;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates the link.
    ///
    Status		Application::Create(Link*		link)
    {
      Method<>			timeout(this, &Application::Timeout);
      Method<const String>	error(this, &Application::Error);

      enter();

      // set the attributes.
      this->state = Application::StateUnconnected;
      this->link = link;

      // create the timer.
      if (this->timer.Create(Timer::ModeSingle, timeout) == StatusError)
	escape("unable to create the timer");

      // register the error callback to the deletion.
      if (this->link->Monitor(error) == StatusError)
	escape("unable to monitor the callback");

      // start the timer.
      if (this->timer.Start(Application::Expiration) == StatusError)
	escape("unable to start the timer");

      leave();
    }

    ///
    /// this method destroys the application.
    ///
    Status		Application::Destroy()
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

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this callbacks is triggered if the time frame expires.
    ///
    Status		Application::Timeout()
    {
      Client*		client;

      enter();

      printf("[XXX] Application::Timeout(0x%x)\n", this);

      // retrieve the client related to this application's link.
      if (Map::Retrieve(this->link, client) == StatusError)
	escape("unable to retrieve the client-application mapping");

      // remove the whole client since the application timed-out.
      if (Client::Remove(client) == StatusError)
	escape("unable to remove the client");

      leave();
    }

    ///
    /// this callbacks is triggered if an error occurs on the link.
    ///
    Status		Application::Error(const String&)
    {
      enter();

      printf("[XXX] Application::Error()\n");

      // XXX remove tout le client.

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
      String		shift(2, ' ');

      enter();

      std::cout << alignment << "[Application]" << std::endl;

      // dump the link.
      if (this->link->Dump(margin + 2) == StatusError)
	escape("unable to dump the link");

      leave();
    }

  }
}
