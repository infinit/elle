//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/shrub/Queue.cc
//
// created       julien quintard   [wed aug 10 03:31:14 2011]
// updated       julien quintard   [thu aug 11 08:28:02 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/shrub/Queue.hh>

namespace etoile
{
  namespace shrub
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    elle::Status	Queue::Add(Riffle*			riffle)
    {
      std::pair<Queue::Iterator, elle::Boolean>	result;

      enter();

      ///
      /// \todo XXX improve this: quite ugly!
      ///   indeed, for every Add(), the loop can go for 50 iterations
      ///   in order to get unique. lists of timestamps should be used.
      ///
      ///   the following printf() can be uncomment to verify this statement.
      ///

      // try to look up the element in the current riffle.
      while (this->container.find(riffle->timestamp) != this->container.end())
	{
	  // XXX printf("Loop\n");

	  // refresh the timestamp.
	  if (riffle->timestamp.Current() == elle::StatusError)
	    escape("unable to retrieve the current time");
	}

      // record the given riffle with its timestamp.
      result =
	this->container.insert(Queue::Value(riffle->timestamp, riffle));

      // check the result.
      if (result.second == false)
	escape("unable to insert the new riffle");

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Queue::Remove(Riffle*			riffle)
    {
      Queue::Iterator	iterator;

      enter();

      // try to look up the riffle in the queue.
      if ((iterator = this->container.find(riffle->timestamp)) ==
	  this->container.end())
	escape("unable to locate the given timestamp");

      // remove the entry from the container.
      this->container.erase(iterator);

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// XXX
    ///
    elle::Status	Queue::Dump(const elle::Natural32	margin) const
    {
      elle::String	alignment(margin, ' ');
      Queue::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Queue] "
		<< std::dec << this->container.size() << std::endl;

      // go through the queue.
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  // display the entry.
	  std::cout << alignment
		    << elle::Dumpable::Shift
		    << "[Entry]" << std::endl;

	  // dump the timestamp.
	  if (scoutor->first.Dump(margin + 4) == elle::StatusError)
	    escape("unable to dump the timestamp");

	  // display the riffle.
	  std::cout << alignment
		    << elle::Dumpable::Shift
		    << elle::Dumpable::Shift
		    << "[Riffle] " << std::hex << scoutor->second << std::endl;
	}

      leave();
    }

  }
}
