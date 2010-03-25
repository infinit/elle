//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/depot/Record.cc
//
// created       julien quintard   [thu dec  3 03:11:13 2009]
// updated       julien quintard   [thu mar 25 18:21:52 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/depot/Record.hh>

namespace etoile
{
  namespace depot
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor
    ///
    Record::Record():
      location(LocationUnknown),
      timer(NULL)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes a record with the address of the block
    /// it is associated.
    ///
    /// this method allocates a timer for blocks which can expires, i.e
    /// mutable blocks, such as PKBs.
    ///
    Status		Record::Create(const hole::Address&	address)
    {
      enter();

      // set the record address.
      this->address = address;

      // check if this family of block expires.
      if (Repository::Delays[address.family] != NULL)
	{
	  Callback<>	discard(&Record::Discard, this);

	  // allocate a new timer object.
	  this->timer = new Timer;

	  // set up the timer.
	  if (this->timer->Create(Timer::ModeSingle,
				  discard) == StatusError)
	    escape("unable to set up the timer");

	  // note that the timer is not started yet. it will be launched
	  // once the Timer() method has been called.
	}

      leave();
    }

    ///
    /// this method must be called whenever the timer is to be re-computed
    /// and restarted.
    ///
    Status		Record::Monitor()
    {
      Time*		time;
      Natural64		expiration;

      enter();

      // if no timer is required for the family of block, just return.
      if (this->timer == NULL)
	leave();

      // stop a potentially already existing timer.
      if (this->timer->Stop() == StatusError)
	escape("unable to stop the timer");

      // retrieve the time.
      time = Repository::Delays[this->address.family];

      // compute the number of seconds. note that day, minute and year
      // are grossly computed.
      expiration = time->second + time->minute * 60 + time->hour * 3600 +
	time->day * 86400 + time->month * 2592000 + time->year * 31104000;

      // start the timer, in milli-seconds.
      if (this->timer->Start(expiration * 1000) == StatusError)
	escape("unable to restart the timer");

      leave();
    }

    ///
    /// this method releases every resource allocated by the record.
    ///
    Status		Record::Destroy()
    {
      enter();

      ///
      /// first, stop and release the existing timer.
      ///
      {
	// release the timer, if there is one.
	if (this->timer != NULL)
	  {
	    // stop the timer.
	    if (this->timer->Stop() == StatusError)
	      escape("unable to stop the timer");

	    // delete it.
	    delete this->timer;
	  }
      }

      ///
      /// then, destroy the data.
      ///
      {
	switch (this->location)
	  {
	  case LocationCache:
	    {
	      // destroy the cell.
	      if (this->data.cell->Destroy() == StatusError)
		escape("unable to destroy the cell");

	      // release the cell.
	      delete this->data.cell;

	      break;
	    }
	  case LocationReserve:
	    {
	      // destroy the unit.
	      if (this->data.unit->Destroy() == StatusError)
		escape("unable to destroy the unit");

	      // release the unit.
	      delete this->data.unit;

	      break;
	    }
	  case LocationUnknown:
	    {
	      escape("unable to locate the data");
	    }
	  }
      }

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the record attributes.
    ///
    Status		Record::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Record] "
		<< this << std::endl;

      // dump the address.
      if (this->address.Dump(margin + 2) == StatusError)
	escape("unable to dump the address");

      // dump the content.
      switch (this->location)
	{
	case LocationCache:
	  {
	    if (this->data.cell->Dump(margin + 2) == StatusError)
	      escape("unable to dump the cell");

	    break;
	  }
	case LocationReserve:
	  {
	    if (this->data.unit->Dump(margin + 2) == StatusError)
	      escape("unable to dump the unit");

	    break;
	  }
	case LocationUnknown:
	  {
	    escape("unknown location");
	  }
	}
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this method is called whenever the element timeouts i.e must be
    /// removed from the repository.
    ///
    Status		Record::Discard()
    {
      enter();

      // remove the block from the repository.
      if (Repository::Discard(this->address) == StatusError)
	escape("unable to discard the timeout block");

      leave();
    }

  }
}
