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
// updated       julien quintard   [fri jan 29 11:03:15 2010]
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
      // set the record address.
      this->address = address;

      // check if this family of block expires.
      if (Repository::Delays[address.family] != NULL)
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
    /// this method must be called whenever the timer is to be re-computed
    /// and restarted.
    ///
    Status		Record::Timer()
    {
      Time*		time;
      Natural64		expiration;

      // if no timer is required for the family of block, just return.
      if (this->timer == NULL)
	leave();

      // stop a potentially already existing timer.
      this->timer->stop();

      // retrieve the time.
      time = Repository::Delays[this->address.family];

      // compute the number of seconds. note that day, minute and year
      // are grossly computed.
      expiration = time->second + time->minute * 60 + time->hour * 3600 +
	time->day * 86400 + time->month * 2592000 + time->year * 31104000;

      // start the timer, in milli-seconds.
      this->timer->start(expiration * 1000);

      leave();
    }

    ///
    /// this method releases every resource allocated by the record.
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
    }

//
// ---------- slots -----------------------------------------------------------
//

    ///
    /// this method is called whenever the element timeouts i.e must be
    /// removed from the repository.
    ///
    void		Record::Timeout()
    {
      // remove the block from the repository.
      if (Repository::Discard(this->address) == StatusError)
	alert("unable to discard the timeout block");
    }

  }
}
