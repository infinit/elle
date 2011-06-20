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
// updated       julien quintard   [sun jun 19 17:59:28 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/depot/Repository.hh>
#include <etoile/depot/Cell.hh>
#include <etoile/depot/Unit.hh>

#include <etoile/configuration/Configuration.hh>

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
    elle::Time*				Repository::Delays[nucleus::Families];

    ///
    /// the data container.
    ///
    Repository::Data::Container		Repository::Container;

    ///
    /// the cache attributes: size, capacity and LRU list.
    ///
    elle::Natural64			Repository::Cache::Size = 0;
    elle::Natural64			Repository::Cache::Capacity;
    Repository::Access::Container	Repository::Cache::Queue;

    ///
    /// the reserve attributes: size, capacity and LRU list.
    ///
    elle::String			Repository::Reserve::Path;
    elle::Natural64			Repository::Reserve::Size = 0;
    elle::Natural64			Repository::Reserve::Capacity;
    Repository::Access::Container	Repository::Reserve::Queue;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes the repository.
    ///
    elle::Status	Repository::Initialize()
    {
      enter();

      //
      // set the static definitions.
      //
      {
	Repository::Cache::Capacity =
	  configuration::Configuration::Cache::Capacity;

	Repository::Reserve::Path = "XXX: depends upon the network";
	  // XXX lune::Lune::User::Reserve;

	Repository::Reserve::Capacity =
	  configuration::Configuration::Reserve::Capacity;
      }

      //
      // set the delays
      //
      {
	// set the content hash block to NULL since such blocks never expire.
	Repository::Delays[nucleus::FamilyContentHashBlock] = NULL;

	// set the public key blocks delay.
	Repository::Delays[nucleus::FamilyPublicKeyBlock] = new elle::Time;
	Repository::Delays[nucleus::FamilyPublicKeyBlock]->minute = 5;
      }

      leave();
    }

    ///
    /// this method cleans the repository by releasing the resources used.
    ///
    elle::Status	Repository::Clean()
    {
      Repository::Data::Scoutor		i;
      Repository::Access::Scoutor	j;

      enter();

      // release the data resources.
      for (i = Repository::Container.begin();
	   i != Repository::Container.end();
	   i++)
	{
	  Record*	record = i->second;

	  // destroy the record.
	  if (record->Destroy() == elle::StatusError)
	    escape("unable to dump the record");

	  // delete the record.
	  delete record;
	}

      // remove all the data elements.
      Repository::Container.clear();

      // remove all the access elements.
      Repository::Cache::Queue.clear();
      Repository::Reserve::Queue.clear();

      // delete the delay for the public key block family.
      delete Repository::Delays[nucleus::FamilyPublicKeyBlock];

      leave();
    }

    ///
    /// this method updates the repository by storing a new block.
    ///
    elle::Status	Repository::Put(const nucleus::Address&	address,
					nucleus::Block*		block)
    {
      std::pair<Repository::Data::Iterator, elle::Boolean>	result;
      Repository::Data::Iterator				iterator;
      Record*							record;

      enter(instance(record));

      //
      // first, retrieve the existing record or create a new one.
      //
      {
	// look for an existing record.
	if ((iterator = Repository::Container.find(address)) ==
	    Repository::Container.end())
	  {
	    // if none exists, allocate a new one.
	    record = new Record;

	    // create the record.
	    if (record->Create(address) == elle::StatusError)
	      escape("unable to create the record");

	    // insert the record.
	    result =
	      Repository::Container.insert(Repository::Data::Value(address,
								   record));

	    // check the result.
	    if (result.second == false)
	      escape("unable to insert the block");

	    // stop tracking record.
	    waive(record);
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
		  Repository::Cache::Queue.remove(record);

		  break;
		}
	      case LocationReserve:
		{
		  Repository::Reserve::Queue.remove(record);

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
      // then, evict data so that the new block ca be stored.
      //
      {
	elle::Natural32	size;

	// retrieve the size of the object.
	if (block->Imprint(size) == elle::StatusError)
	  escape("unable to retrieve the object's size");

	// evict enough data from the cache so that the block can be stored.
	if (Repository::Evict(LocationCache, size) == elle::StatusError)
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
	      if (record->cell->Set(block) == elle::StatusError)
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
	      if (record->unit->Destroy() == elle::StatusError)
		escape("unable to destroy the unit");

	      // release the unit.
	      delete record->unit;

	      // allocate a new cell.
	      record->cell = new Cell;

	      // create a cell.
	      if (record->cell->Set(block) == elle::StatusError)
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
	      record->cell = new Cell;

	      // create the cell.
	      if (record->cell->Set(block) == elle::StatusError)
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
	Repository::Cache::Queue.push_back(record);

	// re-compute the expiration timer, if required.
	if (record->Monitor() == elle::StatusError)
	  escape("unable to reset the timer");
      }

      leave();
    }

    ///
    /// this method retrieve a data block, possibly from the cache or the
    /// reserve.
    ///
    elle::Status	Repository::Get(const nucleus::Address&	address,
					nucleus::Block*&	block)
    {
      Record*		record;

      enter();

      //
      // first, the block is retrieve from the repository, either located
      // in the cache or in the reserve.
      //
      {
	Repository::Data::Iterator	iterator;

	// look for an existing record.
	if ((iterator = Repository::Container.find(address)) ==
	    Repository::Container.end())
	  false();

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
	      if (record->cell->Get(block) == elle::StatusError)
		escape("unable to get the block's data");

	      // remove the access stamp.
	      Repository::Cache::Queue.remove(record);

	      break;
	    }
	  case LocationReserve:
	    {
	      elle::Natural32	size;

	      //
	      // if the data is in the reserve, get the data, destroy the
	      // unit and create a new cell.
	      //

	      // remove the access stamp, especially for the upcoming eviction.
	      Repository::Reserve::Queue.remove(record);

	      // retrieve the size of the object.
	      if (block->Imprint(size) == elle::StatusError)
		escape("unable to retrieve the object's size");

	      // evict enough data from the cache so that the block
	      // can be stored.
	      if (Repository::Evict(LocationCache, size) == elle::StatusError)
		escape("unable to evict enough data");

	      // get the block's data from the unit.
	      if (record->unit->Get(block) == elle::StatusError)
		escape("unable to get the block's data");

	      // destroy the unit.
	      if (record->unit->Destroy() == elle::StatusError)
		escape("unable to destroy the unit");

	      // release the unit.
	      delete record->unit;

	      // allocate a new cell.
	      record->cell = new Cell;

	      // create a cell.
	      if (record->cell->Set(block) == elle::StatusError)
		escape("unable to set the cell");

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
	Repository::Cache::Queue.push_back(record);

	// re-compute the expiration timer.
	if (record->Monitor() == elle::StatusError)
	  escape("unable to reset the timer");
      }

      true();
    }

    ///
    /// this method removes the given block from repository.
    ///
    elle::Status	Repository::Discard(const nucleus::Address& address)
    {
      Repository::Data::Iterator	iterator;
      Record*				record;

      enter(instance(record));

      // look for the address.
      if ((iterator = Repository::Container.find(address)) ==
	  Repository::Container.end())
	escape("unable to locate the address");

      // retrieve the record.
      record = iterator->second;

      // erase the element.
      Repository::Container.erase(iterator);

      // remove the associated stamp.
      switch (record->location)
	{
	case LocationCache:
	  {
	    Repository::Cache::Queue.remove(record);

	    break;
	  }
	case LocationReserve:
	  {
	    Repository::Reserve::Queue.remove(record);

	    break;
	  }
	case LocationUnknown:
	  {
	    escape("unknown location");
	  }
	}

      // destroy the record's content.
      if (record->Destroy() == elle::StatusError)
	escape("unable to destroy the record");

      // stop tracking.
      waive(record);

      // delete the record.
      delete record;

      leave();
    }

    ///
    /// this method removes the least recently used elements until the
    /// given repository, cache or reserve, has enough free space.
    ///
    elle::Status	Repository::Evict(const Location	location,
					  const elle::Natural32	length)
    {
      elle::Natural64*			size;
      const elle::Natural64*		capacity;
      Repository::Access::Container*	access;

      enter();

      // retrieve the information related to the repository to treat, cache
      // or reserve, such as the size, capacity and access list.
      switch (location)
	{
	case LocationCache:
	  {
	    size = &Repository::Cache::Size;
	    capacity = &Repository::Cache::Capacity;
	    access = &Repository::Cache::Queue;

	    break;
	  }
	case LocationReserve:
	  {
	    size = &Repository::Reserve::Size;
	    capacity = &Repository::Reserve::Capacity;
	    access = &Repository::Reserve::Queue;

	    break;
	  }
	case LocationUnknown:
	  {
	    escape("unknown location");
	  }
	}

      // evict data from the repository until there is enough room for
      // the upcoming insertion.
      while ((*size + (elle::Natural64)length) > *capacity)
	{
	  Record*			record;
	  Repository::Data::Iterator	iterator;
	  nucleus::Block*		block;

	  // retrieve the record at the front i.e the least recently accessed.
	  record = access->front();

	  // find the element.
	  if ((iterator = Repository::Container.find(record->address)) ==
	      Repository::Container.end())
	    escape("unable to find the record associated with the LRU stamp");

	  // the following migrates the data to a larger repository.
	  switch (record->location)
	    {
	    case LocationCache:
	      {
		Unit*	unit;

		enter(instance(unit));

		// retrieve the block.
		if (record->cell->Get(block) == elle::StatusError)
		  escape("unable to retrieve the block from the cell");

		// new unit.
		unit = new Unit;

		// set the unit.
		if (unit->Set(block) == elle::StatusError)
		  escape("unable to set the unit");

		// destroy the cell.
		if (record->cell->Destroy() == elle::StatusError)
		  escape("unable to destroy the cell");

		// release the cell.
		delete record->cell;

		// attach the unit.
		record->unit = unit;

		// stop tracking the unit.
		waive(unit);

		// set the location.
		record->location = LocationReserve;

		// add a stamp to the reserve access list.
		Repository::Reserve::Queue.push_back(record);

		release();

		break;
	      }
	    case LocationReserve:
	      {
		//
		// note that, in this case, the block is not pushed further
		// in the hole.
		//
		// indeed, also the repository may hold modified blocks
		// waiting to be pushed to the hole, it is the journal's
		// responsability to update the hole.
		//
		// one of the reason for that is that the journal has all
		// the information for resolving upcoming conflicts. besides,
		// by letting the repository push the modifications in the
		// hole, the system may end up never pushing those
		// modifications, for example if the block keeps being
		// accessed. indeed, a block being constantly accessed would
		// stay in cache, hence would never be transfered from
		// the reserve to the hole, hence the modified block would
		// never be published.
		//

		// destroy the record, including the unit.
		if (record->Destroy() == elle::StatusError)
		  escape("unable to destroy the record");

		// remove the record from the repository.
		Repository::Container.erase(iterator);

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

    ///
    /// this method dumps the whole repository.
    ///
    elle::Status	Repository::Show(const elle::Natural32	margin)
    {
      elle::String			alignment(margin, ' ');
      Repository::Data::Scoutor		i;
      Repository::Access::Scoutor	j;

      enter();

      std::cout << alignment << "[Repository]" << std::endl;

      std::cout << alignment << elle::Dumpable::Shift
		<< "[Cache]" << std::endl;
      std::cout << alignment << elle::Dumpable::Shift
		<< elle::Dumpable::Shift << "[Size] "
		<< std::dec << Repository::Cache::Size << std::endl;
      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[Capacity] " << std::dec << Repository::Cache::Capacity
		<< std::endl;

      std::cout << alignment << elle::Dumpable::Shift
		<< "[Reserve]" << std::endl;
      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[Size] "
		<< std::dec << Repository::Reserve::Size << std::endl;
      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[Capacity] " << std::dec << Repository::Reserve::Capacity
		<< std::endl;

      //
      // dump the data.
      //
      std::cout << alignment << elle::Dumpable::Shift << "[Data]" << std::endl;

      for (i = Repository::Container.begin();
	   i != Repository::Container.end();
	   i++)
	{
	  Record*	record = i->second;

	  // dump the record.
	  if (record->Dump(margin + 4) == elle::StatusError)
	    escape("unable to dump the record");
	}

      //
      // dump the access.
      //
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Access]" << std::endl;

      // first, the cache accesses.
      std::cout << alignment << elle::Dumpable::Shift << elle::Dumpable::Shift
		<< "[Cache]" << std::endl;

      for (j = Repository::Cache::Queue.begin();
	   j != Repository::Cache::Queue.end();
	   j++)
	{
	  Record*	record = *j;

	  // dump the record's address.
	  std::cout << alignment << elle::Dumpable::Shift
		    << elle::Dumpable::Shift << elle::Dumpable::Shift
		    << "[Pointer] " << record << std::endl;
	}

      // second, the reserve accesses.
      std::cout << alignment << elle::Dumpable::Shift
		<< elle::Dumpable::Shift
		<< "[Reserve]" << std::endl;

      for (j = Repository::Reserve::Queue.begin();
	   j != Repository::Reserve::Queue.end();
	   j++)
	{
	  Record*	record = *j;

	  // dump the record's address.
	  std::cout << alignment << elle::Dumpable::Shift
		    << elle::Dumpable::Shift << elle::Dumpable::Shift
		    << "[Pointer] " << record << std::endl;
	}

      leave();
    }

  }
}
