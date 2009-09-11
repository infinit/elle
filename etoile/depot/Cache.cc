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
// updated       julien quintard   [fri sep 11 01:28:53 2009]
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
    Cache::Block::Container	Cache::Blocks;

    ///
    /// the list of LRU elements.
    ///
    Cache::Stamp::Container	Cache::Stamps;

    ///
    /// the current number of cached elements.
    ///
    Natural32			Cache::Size = 0;

    ///
    /// the cache capacity in numbers of elements.
    ///
    Natural32			Cache::Capacity = 64;

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
      Cache::Block::Iterator	iterator;

      // clean the stamps container.
      Cache::Stamps.clear();

      // release the blocks.
      for (iterator = Cache::Blocks.begin();
	   iterator != Cache::Blocks.end();
	   iterator++)
	{
	  // delete the block.
	  delete iterator->second.block;
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
      Cache::Block::Iterator	i;
      Cache::Stamp::Iterator	j;

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

      // remove the element from the stamp
      {
	Cache::Stamps.remove(address);
      }

      // update the cache size.
      Cache::Size--;

      leave();
    }

    ///
    /// this method removes a block from the cache because it has expires.
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
    /// this method takes the least recently used block and removes it
    /// from the cache, if needed.
    ///
    /// then
    ///
    Status		Cache::Evict()
    {
      hole::Address		address;
      hole::Block*		block;
      Cache::Block::Iterator	iterator;
      core::Time		expiration;
      core::Time		time;

      // if the cache is disabled or there is available space, return.
      if ((Cache::Capacity == 0) ||
	  (Cache::Size < Cache::Capacity))
	leave();

      // retrieve the front stamp.
      address = Cache::Stamps.front();

      // find the element.
      if ((iterator = Cache::Blocks.find(address)) == Cache::Blocks.end())
	escape("an unexpected error occured");

      // retrieve the expiration date.
      expiration = iterator->second.expiration;

      // destroy the element.
      if (Cache::Destroy(address) == StatusError)
	escape("unable to destroy the block");

      // get the current time.
      if (time.Current() == StatusError)
	escape("unable to get the current time");

      // if the block is not expired, forward it into the house.
      if (time < expiration)
	;
      // XXX House::Put(address, block, expiration)

      leave();
    }

    ///
    /// this method dumps the cache contents.
    ///
    Status		Cache::Dump(const Natural32		margin)
    {
      String			alignment(margin, ' ');
      String			shift(2, ' ');
      Cache::Block::Scoutor	bs;
      Cache::Stamp::Scoutor	ss;

      std::cout << alignment << "[Cache]" << std::endl;

      std::cout << alignment << shift << "[Size] "
		<< Cache::Size << std::endl;
      std::cout << alignment << shift << "[Capacity] "
		<< Cache::Capacity << std::endl;

      //
      // dump the blocks
      //
      std::cout << alignment << shift << "[Blocks]" << std::endl;

      for (bs = Cache::Blocks.begin();
	   bs != Cache::Blocks.end();
	   bs++)
	{
	  hole::Address		address = bs->first;
	  Cache::Block		block = bs->second;

	  if (address.Dump(margin + 4) == StatusError)
	    escape("unable to dump the address");

	  if (block.expiration.Dump(margin + 6) == StatusError)
	    escape("unable to dump the expiration date");

	  std::cout << alignment << shift << shift << shift
		    << "[Block] " << block.block << std::endl;
	}

      //
      // dump the stamps
      //
      std::cout << alignment << shift << "[Stamps]" << std::endl;

      for (ss = Cache::Stamps.begin();
	   ss != Cache::Stamps.end();
	   ss++)
	{
	  hole::Address		address = *ss;

	  if (address.Dump(margin + 4) == StatusError)
	    escape("unable to dump the address");
	}

      leave();
    }

  }
}
