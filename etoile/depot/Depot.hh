//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/depot/Depot.hh
//
// created       julien quintard   [tue sep  1 01:08:05 2009]
// updated       julien quintard   [thu jan  7 13:16:51 2010]
//

#ifndef ETOILE_DEPOT_DEPOT_HH
#define ETOILE_DEPOT_DEPOT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/hole/Hole.hh>
#include <etoile/core/Core.hh>

#include <etoile/depot/Record.hh>

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
    /// this class abstracts the storage layer into three units: the cache
    /// which keeps blocks in main memory, the house which keeps block
    /// on a stable but slower storage device such as on a hard disk and
    /// the hole which represents the online storage space.
    ///
    /// note that blocks are passed as pointers for two reasons. firstly,
    /// without pointers, the compiler loses the derived class of the
    /// passed block and such a type is required to make a copy of the
    /// block in order to store it in a C++ container. secondly, by relying
    /// on block pointers everywhere, no allocations nor copies are required,
    /// leading to better performance.
    ///
    /// furthermore, the depot---hence the underlying cache, house and
    /// hole---are expecting to take blocks allocated through the new
    /// operator. likewise, the Get() method returns an already
    /// new-operator-allocated block.
    ///
    class Depot
    {
    public:
      //
      // structures
      //
      struct		Cache
      {
	//
	// types
	//
	typedef std::pair<hole::Address, Record*>	Value;
	typedef std::map<hole::Address, Record*>	Container;
	typedef Container::iterator			Iterator;
	typedef Container::const_iterator		Scoutor;
      };

      struct		Access
      {
	//
	// types
	//
	typedef std::list<Record*>			Container;
	typedef Container::iterator			Iterator;
	typedef Container::const_iterator		Scoutor;
      };

      //
      // globals
      //
      static core::Time		Delays[hole::Families];

      //
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();

      static Status	Put(const hole::Address&,
			    hole::Block*);
      static Status	Get(const hole::Address&,
			    hole::Block*&);

      //
      // templates
      //

      ///
      /// this method has been introduced because the C++ typing system
      /// seems unable to implicitly cast an core::Object* for instance
      /// to a hole::Block*& or to const-equivalents.
      ///
      template <typename T>
      static Status	Put(const hole::Address&		address,
			    T*					block)
      {
	// just forward the call to the appropriate method with the correct
	// types.
	return (Depot::Put(address, (hole::Block*)block));
      }

      template <typename T>
      static Status	Get(const hole::Address&		address,
			    T*&					block)
      {
	// just forward the call to the appropriate method with the correct
	// types.
	return (Depot::Get(address, (hole::Block*&)block));
      }
    };

  }
}

#endif
