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
// updated       julien quintard   [fri mar 19 22:21:42 2010]
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
    const Natural32		Guest::Expiration = 2000;

    ///
    /// this container holds the unconnected and auauthentified links also
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
      link(NULL)
    {
    }

    ///
    /// destructor.
    ///
    Guest::~Guest()
    {
      // release the link if needed.
      if (((this->options & Guest::OptionDetach) == 0) &&
	  (this->link != NULL))
	delete this->link;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a guest by recording the link and setting
    /// a timer.
    ///
    Status		Guest::Create(Link*			link)
    {
      Method<>			timeout(this, &Guest::Timeout);
      Method<const String>	error(this, &Guest::Error);

      enter();

      // set the link attribute.
      this->link = link;

      // create the timer.
      if (this->timer.Create(Timer::ModeSingle, timeout) == StatusError)
	escape("unable to create the timer");

      // register the error callback to the guest deletion.
      if (this->link->Monitor(error) == StatusError)
	escape("unable to monitor the callback");

      // start the timer.
      if (this->timer.Start(Guest::Expiration) == StatusError)
	escape("unable to start the timer");

      leave();
    }

    ///
    /// this method creates a guest by recording the link and setting
    /// a timer.
    ///
    Status		Guest::Destroy()
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
    /// this method detaches the link from the guest so that it does
    /// not get deleted along with the guest.
    ///
    Status		Guest::Detach()
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
    Status		Guest::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Guest] " << std::hex << this << std::endl;

      // dump the link.
      if (this->link->Dump(margin + 2) == StatusError)
	escape("unable to dump the link");

      leave();
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this callbacks is triggered if the time frame expires.
    ///
    Status		Guest::Timeout()
    {
      enter();

      // remove the guest from the queue.
      if (Guest::Remove(this) == StatusError)
	escape("unable to remove the guest from the queue");

      leave();
    }

    ///
    /// this callbacks is triggered if an error occurs on the link.
    ///
    Status		Guest::Error(const String&)
    {
      enter();

      // remove the guest from the queue.
      if (Guest::Remove(this) == StatusError)
	escape("unable to remove the guest from the queue");

      leave();
    }

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the guest container.
    ///
    Status		Guest::Initialize()
    {
      enter();

      // nothing to do

      leave();
    }

    ///
    /// this method cleans the guest container.
    ///
    Status		Guest::Clean()
    {
      enter();

      // remove every remaining guest.
      while (Guest::Guests.empty() == false)
	Guest::Remove(Guest::Guests.front());

      leave();
    }

    ///
    /// this method adds a guest to the container.
    ///
    Status		Guest::Add(Guest*			guest)
    {
      enter();

      // add the guest to the list.
      Guest::Guests.push_front(guest);

      leave();
    }

    ///
    /// this method tries to locate a guest based on its link.
    ///
    Status		Guest::Locate(Link*			link,
				      Guest::Iterator&		iterator)
    {
      enter();

      // look for the link.
      for (iterator = Guest::Guests.begin();
	   iterator != Guest::Guests.end();
	   iterator++)
	if (link = (*iterator)->link)
	  true();

      false();
    }

    ///
    /// this method returns a guest.
    ///
    Status		Guest::Retrieve(Link*			link,
					Guest*&			guest)
    {
      Guest::Iterator	iterator;

      enter();

      // locate the guest.
      if (Guest::Locate(link, iterator) != StatusTrue)
	escape("unable to locate the link");

      // return the value.
      guest = *iterator;

      leave();
    }

    ///
    /// this method destroyes a guest.
    ///
    Status		Guest::Remove(Guest*			guest)
    {
      Guest::Iterator	iterator;

      enter();

      // locate the guest in the list.
      if (Guest::Locate(guest->link, iterator) != StatusTrue)
	escape("unable to locate the guest");

      // remove the guest.
      Guest::Guests.erase(iterator);

      // destroy the guest.
      if (guest->Destroy() == StatusError)
	escape("unable to destroy the guest");

      // delete the guest.
      delete guest;

      leave();
    }

    ///
    /// this method dumps the guests..
    ///
    Status		Guest::Show(const Natural32		margin)
    {
      String		alignment(margin, ' ');
      Guest::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Guests]" << std::endl;

      // dump the guests.
      for (scoutor = Guest::Guests.begin();
	   scoutor != Guest::Guests.end();
	   scoutor++)
	if ((*scoutor)->Dump(margin + 2) == StatusError)
	  escape("unable to dump the guest");

      leave();
    }

  }
}
