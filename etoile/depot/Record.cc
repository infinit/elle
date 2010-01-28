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
// updated       julien quintard   [thu jan 28 00:54:29 2010]
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
      timer(NULL),
      size(0)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    Status		Record::Create(const hole::Address&	address,
				       const Natural32		size)
    {
      // set the record address.
      this->address = address;

      // set the block's size when it is active i.e in main memory.
      this->size = size;

      // check if this family of block expires.
      if (Repository::delay[address.family] != NULL)
	{
	  // allocate a new timer object.
	  this->timer = new ::QTimer;

	  // connect the timeout signal.
	  if (this->connect(this->timer, SIGNAL(timeout()),
			    this, SLOT(Timeout())) == false)
	    escape("unable to connect the timeout signal");

	  // note that the timer is not started yet. it will be launched
	  // once the Timer() method has been called.
	}

      leave();
    }

    ///
    /// XXX
    ///
    Status		Record::Timer()
    {
      Time		time;
      Natural64		expiration;

      // if no timer is required for the family of block, just return.
      if (this->timer == NULL)
	leave();

      // stop a potentially already existing timer.
      this->timer->stop();

      // compute the current time.
      if (time.Current() == StatusError)
	escape("unable to compute the current time");

      // add the expiration delay for public key blocks.
      time = time + *Repository::delay[this->address.family];

      // compute the number of seconds. note that day, minute and year
      // are ignored.
      expiration = time.second + time.minute * 60 + time.hour * 3600;

      // start the timer.
      this->timer->start(expiration);

      leave();
    }

    ///
    /// XXX
    ///
    Status		Record::Destroy()
    {
      ///
      /// first, stop and release the existing timer.
      ///
      {
	// release the timer, if there is one.
	if (this->timer != NULL)
	  {
	    // stop the timer.
	    this->timer->stop();

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
      String		shift(2, ' ');

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

      /*
      // dump the timer if any.
      if (this->timer != NULL)
	{
	  std::cout << alignment << shift << "[Timer] "
		    << std::dec << this->expiration << std::endl;
	}
      */
    }

//
// ---------- slots -----------------------------------------------------------
//

    ///
    /// this method is called whenever the element timeouts i.e must be
    /// removed from the depot.
    ///
    void		Record::Timeout()
    {
      printf("[XXX] timeout 0x%x\n", this);

      // remove the block from the repository.
      if (Repository::Discard(this->address) == StatusError)
	alert("unable to discard the timeout block");
    }

  }
}
