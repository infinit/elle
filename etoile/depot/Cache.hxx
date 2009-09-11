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
// updated       julien quintard   [fri sep 11 02:14:24 2009]
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
    /// XXX
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

      printf("HERE\n");

      // try to update an existing record.
      if (Cache::Update(address, block, expiration) == StatusOk)
	leave();

      printf("HERE\n");

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
    /// XXX
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
	{
	  printf("NO UPDATE\n");
	false();
	}

      printf("UDATE\n");

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
    /// XXX
    ///
    template <typename T>
    Status		Cache::Get(const hole::Address&		address,
				   T&				block)
    {
      /* XXX test if expired.

      Cache::Block::Iterator	iterator;

      // look for the address.
      if ((iterator = Cache::Blocks.find(address)) == Cache::Blocks.end())
	escape("unable to find the given address in cache");

      // copy the block.
      block = (T&)*(iterator->second);
      */
      leave();
    }

  }
}

#endif
