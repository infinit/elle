//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/depot/Cache.hxx
//
// created       julien quintard   [thu sep 10 13:22:57 2009]
// updated       julien quintard   [sat sep 12 00:49:10 2009]
//

#ifndef ETOILE_DEPOT_CACHE_HXX
#define ETOILE_DEPOT_CACHE_HXX

namespace etoile
{
  namespace depot
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method stores a block in the cache, starting by looking for
    /// an existing record in order to update it.
    ///
    template <typename T>
    Status		Cache::Put(const hole::Address&		address,
				   const T&			block,
				   const core::Time&		expiration)
    {
      std::pair<Cache::Block::Iterator, bool>	result;
      Cache::Block::Iterator			iterator;
      Cache::Block				element;

      // possibly evict the least recently used element to make sure
      // the cache can store this block.
      if (Cache::Evict() == StatusError)
	escape("unable to evict a block");

      // try to update an existing record.
      if (Cache::Update(address, block, expiration) == StatusOk)
	leave();

      // create the new element.
      {
	// set the element attributes.
	element.expiration = expiration;
	element.block = new T(block);
      }

      // store the element.
      {
	// insert the new value.
	result = Cache::Blocks.insert(Cache::Block::Value(address, element));

	// check the result.
	if (result.second == false)
	  escape("unable to insert the block in cache");
      }

      // put the block's address at the head of the freshness list.
      Cache::Stamps.push_front(address);

      // update the cache size.
      Cache::Size++;

      leave();
    }

    ///
    /// this method tries to update an existing record.
    ///
    template <typename T>
    Status		Cache::Update(const hole::Address&	address,
				      const T&			block,
				      const core::Time&		expiration)
    {
      Cache::Block::Iterator	iterator;
      Cache::Block		element;

      // look for an existing record.
      if ((iterator = Cache::Blocks.find(address)) == Cache::Blocks.end())
	false();

      // retrieve the element.
      element = iterator->second;

      // delete the block.
      delete element.block;

      // update the element with a new value.
      element.expiration = expiration;
      element.block = new T(block);

      // refresh the block by removing the old stamp refrence and putting
      // a new one at the front.
      Cache::Stamps.remove(address);

      Cache::Stamps.push_front(address);

      leave();
    }

    ///
    /// this method returns a cached block.
    ///
    template <typename T>
    Status		Cache::Get(const hole::Address&		address,
				   T&				block)
    {
      Cache::Block::Iterator	iterator;
      core::Time		time;

      // look for the address.
      if ((iterator = Cache::Blocks.find(address)) == Cache::Blocks.end())
	false();

      // get the current time.
      if (time.Current() == StatusError)
	escape("unable to retrieve the current time");

      iterator->second.expiration.Dump();
      time.Dump();

      // check if the block has expired.
      if (iterator->second.expiration < time)
	{
	  // discard the block.
	  if (Cache::Discard(address) == StatusError)
	    escape("unable to discard the block");

	  false();
	}

      // copy the block.
      block = *((T*)iterator->second.block);

      leave();
    }

  }
}

#endif
