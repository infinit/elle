//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/depot/Cache.cc
//
// created       julien quintard   [wed sep  9 14:05:06 2009]
// updated       julien quintard   [fri dec  4 12:21:20 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/depot/Cache.hh>

namespace etoile
{
  namespace depot
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// the blocks container.
    ///
    Cache::Repository::Container	Cache::Records;

    ///
    /// the list of LRU elements.
    ///
    Cache::Access::Container		Cache::Stamps;

    ///
    /// the current cache size.
    ///
    Natural32				Cache::Size = 0;

    ///
    /// the cache capacity in number of records.
    ///
    Natural32				Cache::Capacity = 50;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initialises the cache.
    ///
    Status		Cache::Initialize()
    {
      leave();
    }

    ///
    /// this method releases the used resources.
    ///
    Status		Cache::Clean()
    {
      Cache::Repository::Iterator	i;

      // clean the accesses container.
      Cache::Stamps.clear();

      // release the records.
      for (i = Cache::Records.begin();
	   i != Cache::Records.end();
	   i++)
	{
	  // delete the record.
	  delete i->second;
	}

      // clean the records container.
      Cache::Records.clear();

      leave();
    }

    ///
    /// this method removes a block from the cache given its address.
    ///
    Status		Cache::Destroy(const hole::Address&	address)
    {
      Cache::Repository::Iterator	iterator;
      Record*				record;

      // remove the element from the stamps and records
      {
	// look for the address.
	if ((iterator = Cache::Records.find(address)) == Cache::Records.end())
	  escape("unable to locate this block for deletion");

	// retrieve the record.
	record = iterator->second;

	// remove the associated stamp.
	Cache::Stamps.remove(record);

	// delete the record.
	delete record;

	// erase the element.
	Cache::Records.erase(iterator);
      }

      // update the cache size.
      Cache::Size--;

      leave();
    }

    ///
    /// this method removes a block from the cache because it has expired.
    ///
    /// since it is no longer fresh, it does not need to be stored in the
    /// house.
    ///
    Status		Cache::Discard(const hole::Address&	address)
    {
      // destroy the block.
      if (Cache::Destroy(address) == StatusError)
	escape("unable to destroy the block");

      leave();
    }

    ///
    /// this method takes the least recently used block and removes
    /// it from the cache.
    ///
    /// this block it then forwarded to the house.
    ///
    Status		Cache::Evict()
    {
      Record*				record;
      const hole::Block*		block;
      Cache::Repository::Iterator	iterator;
      core::Time			time;

      // if the cache is disabled or there is available space, return.
      if ((Cache::Capacity == 0) ||
	  (Cache::Size < Cache::Capacity))
	leave();

      // retrieve the record at the back i.e the least recently accessed.
      record = Cache::Stamps.back();

      // find the element.
      if ((iterator = Cache::Records.find(record->block->address)) ==
	  Cache::Records.end())
	escape("an unexpected error occured");

      // store the block's address and reset it to NULL so that it is not
      // freed during the Destroy() process.
      block = record->block;
      record->block = NULL;

      // destroy the element.
      if (Cache::Destroy(block->address) == StatusError)
	escape("unable to destroy the block");

      // forward it into the house.
      // XXX destroy MUST not delete the block then! OR find another way!
      // XXX House::Put(address, block, expiration)

      leave();
    }

    ///
    /// this method dumps the cache contents.
    ///
    Status		Cache::Dump(const Natural32		margin)
    {
      String				alignment(margin, ' ');
      String				shift(2, ' ');
      Cache::Repository::Scoutor	i;
      Cache::Access::Scoutor		j;

      std::cout << alignment << "[Cache]" << std::endl;

      std::cout << alignment << shift << "[Size] "
		<< std::dec << Cache::Size << std::endl;
      std::cout << alignment << shift << "[Capacity] "
		<< std::dec << Cache::Capacity << std::endl;

      //
      // dump the records.
      //
      std::cout << alignment << shift << "[Records]" << std::endl;

      for (i = Cache::Records.begin();
	   i != Cache::Records.end();
	   i++)
	{
	  Record*	record = i->second;

	  // dump the address.
	  if (i->first.Dump(margin + 4) == StatusError)
	    escape("unable to dump the address");

	  // dump the record.
	  if (record->Dump(margin + 6) == StatusError)
	    escape("unable to dump the record");
	}

      //
      // dump the stamps.
      //
      std::cout << alignment << shift << "[Stamps]" << std::endl;

      for (j = Cache::Stamps.begin();
	   j != Cache::Stamps.end();
	   j++)
	{
	  Record*	record = *j;

	  // dump the record's address.
	  std::cout << alignment << shift << shift << "[Pointer] "
		    << record << std::endl;
	}

      leave();
    }

    ///
    /// this method stores a block in the cache, starting by looking for
    /// an existing record in order to update it.
    ///
    Status		Cache::Put(const hole::Address&		address,
				   hole::Block*			block,
				   const core::Time&		expiration)
    {
      std::pair<Cache::Repository::Iterator, bool>	result;
      Cache::Repository::Iterator			iterator;
      Record*						record;

      // possibly evict the least recently used element to make sure
      // the cache can store this block.
      if (Cache::Evict() == StatusError)
	escape("unable to evict a block");

      // try to update an existing record.
      if (Cache::Update(address, block, expiration) == StatusOk)
	leave();

      // create the new record.
      {
	// allocate a new record.
	record = new Record;

	// initialize the record.
	if (record->Create(block, expiration) == StatusError)
	  escape("unable to create a new record");
      }

      // store the element.
      {
	// insert the new value.
	result = Cache::Records.insert(Cache::Repository::Value(address,
								record));

	// check the result.
	if (result.second == false)
	  escape("unable to insert the block in cache");
      }

      // put the block's address at the front of the freshness list.
      Cache::Stamps.push_front(record);

      // update the cache size.
      Cache::Size++;

      leave();
    }

    ///
    /// this method tries to update an existing record.
    ///
    Status		Cache::Update(const hole::Address&	address,
				      hole::Block*		block,
				      const core::Time&		expiration)
    {
      Cache::Repository::Iterator	iterator;
      Record*				record;

      // look for an existing record.
      if ((iterator = Cache::Records.find(address)) == Cache::Records.end())
	false();

      // retrieve the record;
      record = iterator->second;

      // re-initialize the record.
      if (record->Update(block, expiration) == StatusError)
	escape("unable to update the cache record");

      // refresh the block by removing the old stamp refrence and putting
      // a new one at the front.
      Cache::Stamps.remove(record);
      Cache::Stamps.push_front(record);

      leave();
    }

    ///
    /// this method returns a cached block.
    ///
    Status		Cache::Get(const hole::Address&		address,
				   hole::Block*&		block)
    {
      Cache::Repository::Iterator	iterator;
      core::Time			time;
      Record*				record;

      // look for the address.
      if ((iterator = Cache::Records.find(address)) == Cache::Records.end())
	false();

      // retrieve the record.
      record = iterator->second;

      // assign the block.
      block = record->block;

      // update the stamps since the block has been accessed.
      Cache::Stamps.remove(record);
      Cache::Stamps.push_front(record);

      leave();
    }

  }
}
