//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/depot/Repository.cc
//
// created       julien quintard   [tue jan 26 14:32:46 2010]
// updated       julien quintard   [thu jan 28 00:55:36 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/depot/Repository.hh>

namespace etoile
{
  namespace depot
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// the expiration delays for blocks depending on their family.
    ///
    core::Time*				Repository::delay[hole::Families];

    ///
    /// the data container.
    ///
    Repository::Data::Container		Repository::data;

    /// XXX
    const Natural64			Repository::Cache::size = 0;
    const Natural64			Repository::Cache::capacity = 2097152;
    Repository::Access::Container	Repository::Cache::access;

    /// XXX
    const Natural64			Repository::Reserve::size = 0;
    const Natural64			Repository::Reserve::capacity = 1073741824;
    Repository::Access::Container	Repository::Reserve::access;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    Status		Repository::Initialize()
    {
      // set the content hash block to NULL since such blocks never expire.
      Repository::delay[hole::FamilyContentHashBlock] = NULL;

      // set the public key blocks delay.
      Repository::delay[hole::FamilyPublicKeyBlock] = new core::Time;
      Repository::delay[hole::FamilyPublicKeyBlock]->minute = 5;

      leave();
    }

    ///
    /// XXX
    ///
    Status		Repository::Clean()
    {
      // XXX remove every record

      leave();
    }

    ///
    /// this method updates the repository by storing a new block.
    ///
    /// XXX revoir description ci-dessous
    /// note that since only updated and new blocks are supposed to arrive
    /// in this method, there is no need to perform verifications: the
    /// past record can be destroyed and a new one can be created.
    ///
    /// indeed, assuming a CHB is stored, it cannot be an updated since
    /// modified CHBs create a new block. assuming it is a PKB, the past cell
    /// or unit must be replaced with a new cell while the expiration time
    /// must be re-computed.
    ///
    Status		Repository::Put(const hole::Address&	address,
					hole::Block*		block,
					Natural32		size)
    {
      std::pair<Repository::Data::Iterator, bool>	result;
      Repository::Data::Iterator			iterator;
      Record*						record;

      //
      // first, retrieve the existing record or create a new one.
      //
      {
	// look for an existing record.
	if ((iterator = Repository::data.find(address)) ==
	    Repository::data.end())
	  {
	    // if none exists, allocate a new one.
	    record = new Record;

	    // create the record.
	    if (record->Create(address, size) == StatusError)
	      escape("unable to create the record");

	    // insert the record.
	    result =
	      Repository::data.insert(Repository::Data::Value(address,
							      record));

	    // check the result.
	    if (result.second == false)
	      escape("unable to insert the block");
	  }
	else
	  {
	    // retrieve the record.
	    record = iterator->second;

	    // remove the access stamp making sure that the eviction
	    // process will not pick that one.
	    switch (record->location)
	      {
	      case LocationCache:
		{
		  Repository::Cache::access.remove(record);

		  break;
		}
	      case LocationReserve:
		{
		  Repository::Reserve::access.remove(record);

		  break;
		}
	      case LocationUnknown:
		{
		  escape("unknown location");
		}
	      }
	  }
      }

      //
      // then, evict data.
      //
      {
	// evict enough data from the cache so that the block can be stored.
	if (Repository::Evict(LocationCache, size) == StatusError)
	  escape("unable to evict enough data");
      }

      //
      // next, the record must be updated with the new block. note that a cell
      // is created no matter what and where were the previous data. indeed,
      // since the data is accessed, it is fresh and must be brought back
      // in main memory.
      //
      {
	switch (record->location)
	  {
	  case LocationCache:
	    {
	      //
	      // if the data is in cache, just update it with the new block
	      // and size.
	      //

	      // update the cell.
	      if (record->data.cell->Set(block) == StatusError)
		escape("unable to set the cell");

	      break;
	    }
	  case LocationReserve:
	    {
	      //
	      // if the data is in the reserve, destroy it and create a
	      // new cell.
	      //

	      // destroy the unit.
	      if (record->data.unit->Destroy() == StatusError)
		escape("unable to destroy the unit");

	      // release the unit.
	      delete record->data.unit;

	      // allocate a new cell.
	      record->data.cell = new Cell;

	      // create a cell.
	      if (record->data.cell->Set(block) == StatusError)
		escape("unable to set the cell");

	      break;
	    }
	  case LocationUnknown:
	    {
	      //
	      // this case means the record is new and should therefore be
	      // properly initialised with a new cell.
	      //

	      // allocate a cell.
	      record->data.cell = new Cell;

	      // create the cell.
	      if (record->data.cell->Set(block) == StatusError)
		escape("unable to set the cell");

	      break;
	    }
	  }

	// finally, set the location to cache.
	record->location = LocationCache;
      }

      //
      // since the data has been accessed, its access stamp must be
      // regenerated while its expiration time must be re-computed.
      //
      {
	// put the block's address at the back of the access list.
	Repository::Cache::access.push_back(record);

	// re-compute the expiration timer, if required.
	if (record->Timer() == StatusError)
	  escape("unable to reset the timer");
      }

      leave();
    }

    ///
    /// XXX
    ///
    Status		Repository::Get(const hole::Address&	address,
					hole::Block*&		block)
    {
      Record*		record;

      //
      // first, the block is retrieve from the repository, either located
      // in the cache or in the reserve.
      //
      {
	Repository::Data::Iterator	iterator;

	// look for an existing record.
	if ((iterator = Repository::data.find(address)) ==
	    Repository::data.end())
	  escape("unable to find the record");

	// retrieve the record.
	record = iterator->second;

	// retrieve the block from either the cache or the house.
	switch (record->location)
	  {
	  case LocationCache:
	    {
	      //
	      // if the data is in cache, just get it and refresh the stamp.
	      //

	      // get the block's data from the cell.
	      if (record->data.cell->Get(block) == StatusError)
		escape("unable to get the block's data");

	      // remove the access stamp.
	      Repository::Cache::access.remove(record);

	      break;
	    }
	  case LocationReserve:
	    {
	      //
	      // if the data is in the reserve, get the data, destroy the
	      // unit and create a new cell.
	      //

	      // get the block's data from the unit.
	      if (record->data.unit->Get(block) == StatusError)
		escape("unable to get the block's data");

	      // destroy the unit.
	      if (record->data.unit->Destroy() == StatusError)
		escape("unable to destroy the unit");

	      // release the unit.
	      delete record->data.unit;

	      // allocate a new cell.
	      record->data.cell = new Cell;

	      // create a cell.
	      if (record->data.cell->Set(block) == StatusError)
		escape("unable to set the cell");

	      // remove the access stamp.
	      Repository::Reserve::access.remove(record);

	      break;
	    }
	  case LocationUnknown:
	    {
	      escape("unknown location");
	    }
	  }
      }

      //
      // since the data has been accessed, its access stamp must be
      // regenerated while its expiration time must be re-computed.
      //
      {
	// put the block's address at the back of the access list.
	Repository::Cache::access.push_back(record);

	// re-compute the expiration timer.
	if (record->Timer() == StatusError)
	  escape("unable to reset the timer");
      }

      leave();
    }

    ///
    /// XXX
    ///
    Status		Repository::Discard(const hole::Address& address)
    {
      Repository::Data::Iterator	iterator;
      Record*				record;

      // look for the address.
      if ((iterator = Repository::data.find(address)) ==
	  Repository::data.end())
	escape("unable to locate the address");

      // retrieve the record.
      record = iterator->second;

      // erase the element.
      Repository::data.erase(iterator);

      // remove the associated stamp.
      switch (record->location)
	{
	case LocationCache:
	  {
	    Repository::Cache::access.remove(record);

	    break;
	  }
	case LocationReserve:
	  {
	    Repository::Reserve::access.remove(record);

	    break;
	  }
	case LocationUnknown:
	  {
	    escape("unknown location");
	  }
	}

      // destroy the record's content.
      if (record->Destroy() == StatusError)
	escape("unable to destroy the record");

      // delete the record.
      delete record;

      leave();
    }

    ///
    /// XXX
    ///
    Status		Repository::Evict(const Location	location,
					  const Natural32	length)
    {
      const Natural64*			size;
      const Natural64*			capacity;
      Repository::Access::Container*	access;

      // retrieve the information related to the repository to treat, cache
      // or reserve, such as the size, capacity and access list.
      switch (location)
	{
	case LocationCache:
	  {
	    size = &Repository::Cache::size;
	    capacity = &Repository::Cache::capacity;
	    access = &Repository::Cache::access;

	    break;
	  }
	case LocationReserve:
	  {
	    size = &Repository::Reserve::size;
	    capacity = &Repository::Reserve::capacity;
	    access = &Repository::Reserve::access;

	    break;
	  }
	case LocationUnknown:
	  {
	    escape("unknown location");
	  }
	}

      // evict data from the repository until there is enough room for
      // the upcoming insertion.
      while ((*size + (Natural64)length) > *capacity)
	{
	  Record*			record;
	  Repository::Data::Iterator	iterator;
	  hole::Block*			block;

	  // retrieve the record at the front i.e the least recently accessed.
	  record = access->front();

	  // find the element.
	  if ((iterator = Repository::data.find(record->address)) ==
	      Repository::data.end())
	    escape("unable to find the record associated with the LRU stamp");

	  // the following migrates the data to a larger repository.
	  switch (record->location)
	    {
	    case LocationCache:
	      {
		Unit*	unit;

		// retrieve the block.
		if (record->data.cell->Get(block) == StatusError)
		  escape("unable to retrieve the block from the cell");

		// new unit.
		unit = new Unit;

		// set the unit.
		if (unit->Set(block) == StatusError)
		  escape("unable to set the unit");

		// destroy the cell.
		if (record->data.cell->Destroy() == StatusError)
		  escape("unable to destroy the cell");

		// release the cell.
		delete record->data.cell;

		// attach the unit.
		record->data.unit = unit;

		// set the location.
		record->location = LocationReserve;

		// add a stamp to the reserve access list.
		Repository::Reserve::access.push_back(record);

		break;
	      }
	    case LocationReserve:
	      {
		// retrieve the data from the unit.
		if (record->data.unit->Get(block) == StatusError)
		  escape("unable to retrieve the block from the unit");

		// store the block in the hole.
		if (hole::Hole::Put(record->address, block) == StatusError)
		  escape("unable to store the block in the hole");

		// destroy the record, including the unit.
		if (record->Destroy() == StatusError)
		  escape("unable to destroy the record");

		// remove the record from the repository.
		Repository::data.erase(iterator);

		// delete the record.
		delete record;

		break;
	      }
	    case LocationUnknown:
	      {
		escape("unable to locate the data");
	      }
	    }

	  // remove the stamp from the access list.
	  access->pop_front();
	}

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the whole repository.
    ///
    Status		Repository::Dump(const Natural32	margin)
    {
      String				alignment(margin, ' ');
      String				shift(2, ' ');
      Repository::Data::Scoutor		i;
      Repository::Access::Scoutor	j;

      std::cout << alignment << "[Repository]" << std::endl;

      std::cout << alignment << shift << "[Cache]" << std::endl;
      std::cout << alignment << shift << shift << "[Size] "
		<< std::dec << Repository::Cache::size << std::endl;
      std::cout << alignment << shift << shift << "[Capacity] "
		<< std::dec << Repository::Cache::capacity << std::endl;

      std::cout << alignment << shift << "[Reserve]" << std::endl;
      std::cout << alignment << shift << shift << "[Size] "
		<< std::dec << Repository::Reserve::size << std::endl;
      std::cout << alignment << shift << shift << "[Capacity] "
		<< std::dec << Repository::Reserve::capacity << std::endl;

      //
      // dump the data.
      //
      std::cout << alignment << shift << "[Data]" << std::endl;

      for (i = Repository::data.begin();
	   i != Repository::data.end();
	   i++)
	{
	  Record*	record = i->second;

	  // dump the record.
	  if (record->Dump(margin + 4) == StatusError)
	    escape("unable to dump the record");
	}

      //
      // dump the access.
      //
      std::cout << alignment << shift << "[Access]" << std::endl;

      // first, the cache accesses.
      std::cout << alignment << shift << shift << "[Cache]" << std::endl;

      for (j = Repository::Cache::access.begin();
	   j != Repository::Cache::access.end();
	   j++)
	{
	  Record*	record = *j;

	  // dump the record's address.
	  std::cout << alignment << shift << shift << shift << "[Pointer] "
		    << record << std::endl;
	}

      // second, the reserve accesses.
      std::cout << alignment << shift << shift << "[Reserve]" << std::endl;

      for (j = Repository::Reserve::access.begin();
	   j != Repository::Reserve::access.end();
	   j++)
	{
	  Record*	record = *j;

	  // dump the record's address.
	  std::cout << alignment << shift << shift << shift << "[Pointer] "
		    << record << std::endl;
	}

      leave();
    }

  }
}

/* XXX

Put[Hole]
 -> new Record
 -> Record.Update
 -> new access
Put[House]
 -> retrieve Record
 -> Record.Update
 -> refresh access
Put[Cache]
 -> retrieve Record
 -> Record.Update
 -> refresh access

Get[Hole]
 -> new Record
 -> Record.Update
 -> new access
Get[House]
 -> retrieve Record
 -> Record.Update
 -> refresh access
Get[Cache]
 -> retrieve Record
 -> refresh access

Discard
 -> retrieve Record
 -> Record.Discard
 -> delete Record

Evict[Cache]
 -> retrieve Record
 -> Record.Evict
Evict[House]
 -> retrieve Record
 -> Record.Evict

-------------------------------------------------------------------------------

Record.Evict()
 => if Cache
  -> new Unit
  -> destroy Cell
 => if Reserve
  -> call Hole
  -> destroy Unit
  -> destroy Record

Record.Update(block, size)
 => if Cache
  -> if same pointer, just update size
 => otherwise
  -> destroy the existing Cell/Unit, if there is
  -> new Cell

Record.Discard()
 -> destroy cell/unit

*/
