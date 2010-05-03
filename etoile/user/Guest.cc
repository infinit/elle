//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/user/Guest.cc
//
// created       julien quintard   [wed mar 17 22:13:51 2010]
// updated       julien quintard   [mon may  3 18:41:24 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/user/Guest.hh>

namespace etoile
{
  namespace user
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this variable defines the time frame within which a guest should
    /// identify/connect or being destroyed.
    ///
    /// this value is in milli-seconds.
    ///
    const elle::Natural32	Guest::Expiration = 2000;

    ///
    /// this container holds the unconnected and auauthentified channels also
    /// known as guests.
    ///
    Guest::Container		Guest::Guests;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Guest::Guest():
      options(Guest::OptionNone),
      channel(NULL)
    {
    }

    ///
    /// destructor.
    ///
    Guest::~Guest()
    {
      // release the channel if needed.
      if (((this->options & Guest::OptionDetach) == 0) &&
	  (this->channel != NULL))
	delete this->channel;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a guest by recording the channel and setting
    /// a timer.
    ///
    elle::Status	Guest::Create(elle::Channel*		channel)
    {
      elle::Callback<>				timeout(&Guest::Timeout, this);
      elle::Callback<const elle::String>	error(&Guest::Error, this);

      enter();

      // set the channel attribute.
      this->channel = channel;

      // create the timer.
      if (this->timer.Create(elle::Timer::ModeSingle,
			     timeout) == elle::StatusError)
	escape("unable to create the timer");

      // register the error callback to the guest deletion.
      if (this->channel->Monitor(error) == elle::StatusError)
	escape("unable to monitor the callback");

      // start the timer.
      if (this->timer.Start(Guest::Expiration) == elle::StatusError)
	escape("unable to start the timer");

      leave();
    }

    ///
    /// this method creates a guest by recording the channel and setting
    /// a timer.
    ///
    elle::Status	Guest::Destroy()
    {
      enter();

      // stop the timer, just in case.
      if (this->timer.Stop() == elle::StatusError)
	escape("unable to stop the timer");

      // withdraw the control management. note however that if the
      // guest's socket has been detached, the monitoring must not
      // withdrawn since the socket is going to be used somewhere else,
      // like in the Agent.
      if ((this->options & Guest::OptionDetach) == 0)
	if (this->channel->Withdraw() == elle::StatusError)
	  escape("unable to withdraw the socket control");

      leave();
    }

    ///
    /// this method detaches the channel from the guest so that it does
    /// not get deleted along with the guest.
    ///
    elle::Status	Guest::Detach()
    {
      enter();

      // set the options.
      this->options = Guest::OptionDetach;

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps a guest.
    ///
    elle::Status	Guest::Dump(const elle::Natural32	margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Guest] " << std::hex << this << std::endl;

      // dump the channel.
      if (this->channel->Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the channel");

      leave();
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this callbacks is triggered if the time frame expires.
    ///
    elle::Status	Guest::Timeout()
    {
      enter();

      // remove the guest from the queue.
      if (Guest::Remove(this) == elle::StatusError)
	escape("unable to remove the guest from the queue");

      leave();
    }

    ///
    /// this callbacks is triggered if an error occurs on the channel.
    ///
    elle::Status	Guest::Error(const elle::String&)
    {
      enter();

      // remove the guest from the queue.
      if (Guest::Remove(this) == elle::StatusError)
	escape("unable to remove the guest from the queue");

      leave();
    }

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the guest container.
    ///
    elle::Status	Guest::Initialize()
    {
      enter();

      // nothing to do

      leave();
    }

    ///
    /// this method cleans the guest container.
    ///
    elle::Status	Guest::Clean()
    {
      enter();

      // remove every remaining guest.
      while (Guest::Guests.empty() == false)
	{
	  Guest*	guest = Guest::Guests.front();

	  // remove the guest.
	  if (Guest::Remove(guest) == elle::StatusError)
	    escape("unable to remove the guest");
	}

      leave();
    }

    ///
    /// this method adds a guest to the container.
    ///
    elle::Status	Guest::Add(Guest*			guest)
    {
      enter();

      // add the guest to the list.
      Guest::Guests.push_front(guest);

      leave();
    }

    ///
    /// this method tries to locate a guest based on its channel.
    ///
    elle::Status	Guest::Locate(elle::Channel*		channel,
				      Guest::Iterator&		iterator)
    {
      enter();

      // look for the channel.
      for (iterator = Guest::Guests.begin();
	   iterator != Guest::Guests.end();
	   iterator++)
	if (channel = (*iterator)->channel)
	  true();

      false();
    }

    ///
    /// this method returns a guest.
    ///
    elle::Status	Guest::Retrieve(elle::Channel*		channel,
					Guest*&			guest)
    {
      Guest::Iterator	iterator;

      enter();

      // locate the guest.
      if (Guest::Locate(channel, iterator) != elle::StatusTrue)
	escape("unable to locate the channel");

      // return the value.
      guest = *iterator;

      leave();
    }

    ///
    /// this method destroyes a guest.
    ///
    elle::Status	Guest::Remove(Guest*			guest)
    {
      Guest::Iterator	iterator;

      enter();

      // locate the guest in the list.
      if (Guest::Locate(guest->channel, iterator) != elle::StatusTrue)
	escape("unable to locate the guest");

      // remove the guest.
      Guest::Guests.erase(iterator);

      // destroy the guest.
      if (guest->Destroy() == elle::StatusError)
	escape("unable to destroy the guest");

      // delete the guest.
      delete guest;

      leave();
    }

    ///
    /// this method dumps the guests..
    ///
    elle::Status	Guest::Show(const elle::Natural32	margin)
    {
      elle::String	alignment(margin, ' ');
      Guest::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Guests]" << std::endl;

      // dump the guests.
      for (scoutor = Guest::Guests.begin();
	   scoutor != Guest::Guests.end();
	   scoutor++)
	if ((*scoutor)->Dump(margin + 2) == elle::StatusError)
	  escape("unable to dump the guest");

      leave();
    }

  }
}
