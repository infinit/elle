//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [wed aug 10 03:31:14 2011]
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
    /// this method adds a riffle to the queue.
    ///
    elle::Status	Queue::Add(Riffle*			riffle)
    {
      Queue::Iterator	iterator;
      Group*		group;

      enter();

      // try to look up a group for the given timestamp.
      if ((iterator = this->container.find(riffle->timestamp)) ==
	  this->container.end())
	{
	  std::pair<Queue::Iterator, elle::Boolean>	result;
	  Group*					g;

	  enterx(instance(g));

	  // allocate a group.
	  g = new Group;

	  // record the group with its timestamp.
	  result =
	    this->container.insert(Queue::Value(riffle->timestamp, g));

	  // check the result.
	  if (result.second == false)
	    escape("unable to insert the new riffle");

	  // set the group since _g_ is going to be reset to NULL.
	  group = g;

	  // waive.
	  waive(g);

	  release();
	}
      else
	{
	  // retrieve the group.
	  group = iterator->second;
	}

      // add the riffle to the group.
      if (group->Add(riffle) == elle::StatusError)
	escape("unable to add the riffle to the group");

      leave();
    }

    ///
    /// this method removes a riffle from the queue.
    ///
    elle::Status	Queue::Remove(Riffle*			riffle)
    {
      Queue::Iterator	iterator;
      Group*		group;

      enter();

      // try to look up the riffle in the queue.
      if ((iterator = this->container.find(riffle->timestamp)) ==
	  this->container.end())
	escape("unable to locate the given timestamp");

      // retrieve the group.
      group = iterator->second;

      // remove the riffle from the group.
      if (group->Remove(riffle) == elle::StatusError)
	escape("unable to remove the riffle");

      // check if the group is empty.
      if (group->container.empty() == true)
	{
	  // delete the group.
	  delete group;

	  // remove the group from the container.
	  this->container.erase(iterator);
	}

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the queue.
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

	  // dump the group.
	  if (scoutor->second->Dump(margin + 4) == elle::StatusError)
	    escape("unable to dump the group");
	}

      leave();
    }

  }
}
