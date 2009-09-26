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
// updated       julien quintard   [sat sep 12 15:24:16 2009]
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
    Cache::Block::Container		Cache::Blocks;

    ///
    /// the list of LRU elements.
    ///
    Cache::Access::Container		Cache::Accesses;

    ///
    /// the list expiration dates.
    ///
    Cache::Expiration::Container	Cache::Expirations;

    ///
    /// the current number of cached elements.
    ///

    // XXX in bytes
    Natural32				Cache::Size = 0;

    ///
    /// the cache capacity in numbers of elements.
    ///

    // XXX in bytes
    Natural32				Cache::Capacity = 64;

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
      Cache::Block::Iterator	i;

      // clean the accesses container.
      Cache::Accesses.clear();

      // clean the expirations container.
      Cache::Expirations.clear();

      // release the blocks.
      for (i = Cache::Blocks.begin();
	   i != Cache::Blocks.end();
	   i++)
	{
	  // delete the block.
	  delete i->second.block;
	}

      // clean the blocks container.
      Cache::Blocks.clear();

      leave();
    }

    ///
    /// this method removes a block from the cache given its address.
    ///
    Status		Cache::Destroy(const hole::Address&	address)
    {
      Cache::Block::Iterator		i;
      Cache::Expiration::Iterator	k;

      // remove the element from the blocks
      {
	// look for the address.
	if ((i = Cache::Blocks.find(address)) == Cache::Blocks.end())
	  false();

	// delete the block.
	delete i->second.block;

	// erase the element.
	Cache::Blocks.erase(i);
      }

      // remove the element from the accesses.
      {
	Cache::Accesses.remove(address);
      }

      // remove the element from the expirations.
      {
	for (k = Cache::Expirations.begin();
	     k != Cache::Expirations.end();
	     k++)
	  {
	    if (k->address == address)
	      {
		Cache::Expirations.erase(k);

		break;
	      }
	  }
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
    /// this method takes an expired or the least recently used block and
    /// removes it from the cache.
    ///
    /// this block, if not expired, it then forward to the house.
    ///
    Status		Cache::Evict()
    {
      hole::Address		address;
      hole::Block*		block;
      core::Time		expiration;
      Cache::Block::Iterator	iterator;
      core::Time		time;

      // if the cache is disabled or there is available space, return.
      if ((Cache::Capacity == 0) ||
	  (Cache::Size < Cache::Capacity))
	leave();

      // get the current time.
      if (time.Current() == StatusError)
	escape("unable to get the current time");

      // try to evict an expired block.
      {
	// if the expirations list contains elements.
	if (Cache::Expirations.size() > 0)
	  {
	    // test if the head block has expired
	    if (Cache::Expirations.begin()->tim < time)
	      {
		if (Cache::Destroy(Cache::Expirations.begin()->address) ==
		    StatusError)
		  escape("unable to destroy the head expired block");

		leave();
	      }
	  }
      }

      // evict the least recently used block.
      {
	// retrieve the front access.
	address = Cache::Stamps.front();

	// find the element.
	if ((iterator = Cache::Blocks.find(address)) == Cache::Blocks.end())
	  escape("an unexpected error occured");

	// retrieve the expiration date.
	//expiration = iterator->second.time;
	// XXX

	// destroy the element.
	if (Cache::Destroy(address) == StatusError)
	  escape("unable to destroy the block");

	// forward it into the house.
	// XXX House::Put(address, block, expiration)
      }

      leave();
    }

    ///
    /// this method dumps the cache contents.
    ///
    Status		Cache::Dump(const Natural32		margin)
    {
      String				alignment(margin, ' ');
      String				shift(2, ' ');
      Cache::Block::Scoutor		i;
      Cache::Access::Scoutor		j;
      Cache::Expiration::Scoutor	k;

      std::cout << alignment << "[Cache]" << std::endl;

      std::cout << alignment << shift << "[Size] "
		<< Cache::Size << std::endl;
      std::cout << alignment << shift << "[Capacity] "
		<< Cache::Capacity << std::endl;

      //
      // dump the blocks.
      //
      std::cout << alignment << shift << "[Blocks]" << std::endl;

      for (i = Cache::Blocks.begin();
	   i != Cache::Blocks.end();
	   i++)
	{
	  hole::Address		address = i->first;
	  hole::Block*		block = i->second;

	  if (address.Dump(margin + 4) == StatusError)
	    escape("unable to dump the address");

	  std::cout << alignment << shift << shift << shift
		    << "[Block] " << block << std::endl;
	}

      //
      // dump the accesses.
      //
      std::cout << alignment << shift << "[Accesses]" << std::endl;

      for (j = Cache::Accesses.begin();
	   j != Cache::Accesses.end();
	   j++)
	{
	  hole::Address		address = *j;

	  if (address.Dump(margin + 4) == StatusError)
	    escape("unable to dump the address");
	}

      //
      // dump the expirations.
      //
      std::cout << alignment << shift << "[Expirations]" << std::endl;

      for (k = Cache::Expirations.begin();
	   k != Cache::Expirations.end();
	   k++)
	{
	  Cache::Expiration	expiration = *ss;

	  if (expiration.address.Dump(margin + 4) == StatusError)
	    escape("unable to dump the address");

	  if (expiration.time.Dump(margin + 4) == StatusError)
	    escape("unable to dump the time");
	}

      leave();
    }

  }
}
