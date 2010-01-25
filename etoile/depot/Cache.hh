//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/depot/Cache.hh
//
// created       julien quintard   [wed sep  9 14:00:24 2009]
// updated       julien quintard   [wed dec 16 17:30:32 2009]
//

#ifndef ETOILE_DEPOT_CACHE_HH
#define ETOILE_DEPOT_CACHE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/hole/Hole.hh>
#include <etoile/core/Core.hh>

#include <etoile/depot/Record.hh>


namespace etoile
{
  namespace depot
  {

//
// ---------- forward declarations --------------------------------------------
//

    class Record;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides a cache mechanism to keep hot blocks in main
    /// memory hence speeding the retrieving process.
    ///
    /// the cache is composed of two data structures: one for storing the
    /// blocks and one for the block's freshness such that whenever a block
    /// is accessed, the block is pushed at the end of this list.
    ///
    /// note that the access list is composed of Address* instead of Address
    /// since Address* are already unique and point to the block's internal
    /// address stored in the first data structure.
    ///
    class Cache
    {
    public:

      //
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();

      static Status	Put(const hole::Address&,
			    hole::Block*,
			    const core::Time&);
      static Status	Update(const hole::Address&,
			       hole::Block*,
			       const core::Time&);
      static Status	Get(const hole::Address&,
			    hole::Block*&);
      static Status	Destroy(const hole::Address&);

      static Status	Discard(const hole::Address&);
      static Status	Evict();

      static Status	Dump(const Natural32 = 0);

      //
      // static attributes
      //
      static Repository::Container	Records;
      static Access::Container		Stamps;

      static Natural32			Size;
      static Natural32			Capacity;
    };

  }
}

#endif
