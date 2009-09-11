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
// updated       julien quintard   [fri sep 11 01:27:04 2009]
//

#ifndef ETOILE_DEPOT_CACHE_HH
#define ETOILE_DEPOT_CACHE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/hole/Hole.hh>
#include <etoile/core/Core.hh>

#include <map>
#include <list>

namespace etoile
{
  namespace depot
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Cache
    {
    public:
      //
      // structures
      //
      struct		Block
      {
	//
	// types
	//
	typedef std::pair<hole::Address, Block>		Value;
	typedef std::map<hole::Address, Block>		Container;
	typedef Container::iterator			Iterator;
	typedef Container::const_iterator		Scoutor;

	//
	// attributes
	//
	core::Time	expiration;
	hole::Block*	block;
      };

      struct		Stamp
      {
	//
	// types
	//
	typedef std::list<hole::Address>		Container;
	typedef Container::iterator			Iterator;
	typedef Container::const_iterator		Scoutor;
      };

      //
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();

      template <typename T>
      static Status	Put(const hole::Address&,
			    const T&,
			    const core::Time&);
      template <typename T>
      static Status	Update(const hole::Address&,
			       const T&,
			       const core::Time&);
      template <typename T>
      static Status	Get(const hole::Address&,
			    T&);
      static Status	Destroy(const hole::Address&);

      static Status	Discard(const hole::Address&);
      static Status	Evict();

      static Status	Dump(const Natural32 = 0);

      //
      // static attributes
      //
      static Block::Container	Blocks;
      static Stamp::Container	Stamps;

      static Natural32		Size;
      static Natural32		Capacity;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <etoile/depot/Cache.hxx>

#endif
