//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/test/network/slot/Table.cc
//
// created       julien quintard   [wed mar 17 13:23:40 2010]
// updated       julien quintard   [mon jul 18 10:00:03 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/test/network/slot/Table.hh>

namespace elle
{
  namespace test
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates the table.
    ///
    Status		Table::Create(Node*			node)
    {
      Callback< Status,
		Parameters<> >	refresh(&Table::Refresh, this);

      enter();

      // set the node.
      this->node = node;

      // create a timer.
      if (this->timer.Create(Timer::ModeRepetition, refresh) == StatusError)
	escape("unable to create the timer");

      // start the timer.
      if (this->timer.Start(Node::Rate) == StatusError)
	escape("unable to start the timer");

      leave();
    }

    ///
    /// this method adds/updates a neighbour.
    ///
    Status		Table::Update(const Address&		address,
				      const String&		name)
    {
      Table::Iterator	iterator;

      enter();

      // try to locate a previous entry.
      if (this->Locate(address, iterator) == StatusOk)
	{
	  // update the neighbour.
	  if ((*iterator)->Update(name) == StatusError)
	    escape("unable to refresh the node");
	}
      else
	{
	  Neighbour*	neighbour;

	  enter(instance(neighbour));

	  // allocate a new neighbour.
	  neighbour = new Neighbour;

	  // create a new neighbour.
	  if (neighbour->Create(this->node, address, name) == StatusError)
	    escape("unable to create the neighbour");

	  // add the neighbour to the list.
	  this->container.push_back(neighbour);

	  // stop tracking.
	  waive(neighbour);

	  release();
	}

      leave();
    }

    ///
    /// this method refreshes an entry.
    ///
    Status		Table::Refresh(const Address&		address)
    {
      Table::Iterator	iterator;

      enter();

      // locate the neighbour.
      if (this->Locate(address, iterator) != StatusOk)
	escape("unable to locate the given address");

      // refresh the neighbour.
      if ((*iterator)->Refresh() == StatusError)
	escape("unable to refresh the node");

      leave();
    }

    ///
    /// this method removes a neighbour.
    ///
    Status		Table::Remove(const Address&		address)
    {
      Table::Iterator	iterator;

      enter();

      // try to locate a previous entry.
      if (this->Locate(address, iterator) == StatusError)
	escape("unable to locate this neighbour");

      // delete the neighbour.
      delete *iterator;

      // remove the element from the list.
      this->container.erase(iterator);

      leave();
    }

    ///
    /// this method locates a neighbour in the list.
    ///
    Status		Table::Locate(const Address&		address,
				      Table::Iterator&		iterator)
    {
      enter();

      // iterator over the container.
      for (iterator = this->container.begin();
	   iterator != this->container.end();
	   iterator++)
	{
	  // if the address is found, return.
	  if ((*iterator)->address == address)
	    leave();
	}

      escape("unable to locate the given neighbour");
    }

    ///
    /// this method merges two table.
    ///
    Status		Table::Merge(const Table&		table)
    {
      Table::Scoutor	scoutor;

      enter();

      // explore the received table.
      for (scoutor = table.container.begin();
	   scoutor != table.container.end();
	   scoutor++)
	{
	  // add/update the table.
	  if (this->Update((*scoutor)->address,
			   (*scoutor)->name) == StatusError)
	    escape("unable to update the table");
	}

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes a table.
    ///
    Status		Table::Serialize(Archive&		archive) const
    {
      Table::Scoutor	scoutor;

      enter();

      // serialize the number of neighbours.
      if (archive.Serialize((Natural32)this->container.size()) == StatusError)
	escape("unable to serialize the neighbour");
      
      // serialize the table by going through it.
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  // serialize the neighbour.
	  if (archive.Serialize(*(*scoutor)) == StatusError)
	    escape("unable to serialize the neighbour");
	}

      leave();
    }

    ///
    /// this method extracts a table.
    ///
    Status		Table::Extract(Archive&		archive)
    {
      Natural32		n;
      Natural32		i;

      enter();

      // extract the number of elements.
      if (archive.Extract(n) == StatusError)
	escape("unable to extract the internal numbers");

      // iterate.
      for (i = 0; i < n; i++)
	{
	  Neighbour*	neighbour;

	  // allocate a neighbour.
	  neighbour = new Neighbour;

	  // extract the neighbour.
	  if (archive.Extract(*neighbour) == StatusError)
	    escape("unable to extract the neighbour");

	  // add the neighbour to the table.
	  this->container.push_back(neighbour);
	}

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the table.
    ///
    Status		Table::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');
      Table::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Table]" << std::endl;

      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  // dump the neighbour.
	  if ((*scoutor)->Dump(margin + 2) == StatusError)
	    escape("unable to dump the neighbour");
	}

      leave();
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this callback is called whenever the state needs refreshing.
    ///
    Status		Table::Refresh()
    {
      Table::Scoutor	scoutor;

      enter();

      //
      // first, dump the table.
      //
      if (this->Dump() == StatusError)
	escape("unable to dump the table");

      //
      // initiate the refreshing process.
      //
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  // send a probe message.
	  if (this->node->slot.Send((*scoutor)->address,
				    Inputs<TagProbe>(this->node->name,
						     this->node->table)) ==
	      StatusError)
	    escape("unable to send a probe");
	}

      leave();
    }

  }
}
