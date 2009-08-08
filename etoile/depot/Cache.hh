//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/cache/Cache.hh
//
// created       julien quintard   [fri jul 31 14:28:15 2009]
// updated       julien quintard   [tue aug  4 13:59:40 2009]
//

#ifndef ETOILE_CACHE_CACHE_HH
#define ETOILE_CACHE_CACHE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/core/Core.hh>

#include <map>

namespace etoile
{
  using namespace core;

  namespace cache
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class is used for storing blocks in main memory in a form
    /// that optimises computing. indeed, blocks are stored unencrypted and
    /// alredy verified.
    ///
    /// mutable blocks remain in the cache for 1 minute by default while
    /// immutable blocks can stay indefinitely until they are evicted.
    ///
    /// note that blocks are supposed to be allocated and the cache takes
    /// over their ownerships hence will manage their deallocation.
    ///
    /// \todo XXX some blocks should be optimised to be mutable but *private*
    /// such that the probability of another entity of the same user updating
    /// the same object at the same time is extremely low. therefore the
    /// system could increase the refresh delay. likewise, objects should
    /// possibly be *local* meaning that nothing would be put online such
    /// as temporary files, object files etc.
    ///
    /// \todo add the concept of User such that a user retrieving a block
    /// from the cache is not authorised to actually access it. indeed, maybe
    /// it was another user who put it in cache.
    ///
    class Cache
    {
      //
      // types
      //
      typedef std::map<String, Block*>		Container;
      typedef Container::iterator		Iterator;

      //
      // static methods
      //
      static Status	Search(String&,
			       Iterator* = NULL);
      static Status	Put(String&,
			    Block*);
      static Status	Get(String&,
			    Block*);
      static Status	Erase(String&);

      //
      // static attributes
      //
      static Container	container;
    };

  }
}

#endif
