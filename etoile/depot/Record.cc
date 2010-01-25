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
// updated       julien quintard   [thu jan  7 13:19:41 2010]
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
      block(NULL),
      timer(NULL)
    {
    }

    ///
    /// destructor
    ///
    Record::~Record()
    {
      // delete the block.
      if (this->block != NULL)
	delete this->block;

      // delete the timer.
      if (this->timer != NULL)
	delete this->timer;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a record.
    ///
    Status		Record::Create(hole::Block*		block,
				       const core::Time&	expiration)
    {
      // set the block.
      this->block = block;

      // check if expiration is equal to infinite. if not, set a timer.
      if (expiration.year == Variable::Maximum(expiration.year))
	{
	  // set the timer to NULL;
	  this->timer = NULL;

	  leave();
	}

      // construct a new timer.
      this->timer = new ::QTimer();

      // connect the timeout signal.
      if (this->connect(this->timer, SIGNAL(timeout()),
			this, SLOT(Timeout())) == false)
	escape("unable to connect the timeout signal");

      // compute the number of seconds. note that day, minute and year
      // are ignored.
      this->expiration =
	expiration.second +
	expiration.minute * 60 +
	expiration.hour * 3600;

      // XXX to test
      this->expiration = 5000;

      // start the timer.
      this->timer->start(this->expiration);

      leave();
    }

    ///
    /// this method updates a record. this method is used in the Update()
    /// cache method instead of deleting and allocating a new record because
    /// that would require to also update the Stamps structure which points
    /// to the associated record.
    ///
    Status		Record::Update(hole::Block*		block,
				       const core::Time&	expiration)
    {
      // stop and free the timer if there is one.
      if (this->timer != NULL)
	{
	  // stop it.
	  this->timer->stop();

	  // free it.
	  delete this->timer;
	}

      // free the block.
      if (this->block != NULL)
	delete this->block;

      // finally, re-initialize the record.
      if (this->Create(block, expiration) == StatusError)
	escape("unable to re-initialize the record");

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

      std::cout << alignment << shift << "[Block] "
		<< this->block << std::endl;

      // dump the timer if any.
      if (this->timer != NULL)
	{
	  std::cout << alignment << shift << "[Timer] "
		    << std::dec << this->expiration << std::endl;
	}
    }

//
// ---------- slots -----------------------------------------------------------
//

    ///
    /// this method is called whenever the element timeouts i.e must be
    /// removed from the cache.
    ///
    void		Record::Timeout()
    {
      /* XXX
      // remove the block from the cache.
      if (Cache::Discard(block->address) == StatusError)
	alert("unable to discard the timeout block");
      */
    }

  }
}
