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
// updated       julien quintard   [fri dec  4 19:12:20 2009]
//

#ifndef ETOILE_DEPOT_DEPOT_HH
#define ETOILE_DEPOT_DEPOT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/hole/Hole.hh>
#include <etoile/core/Core.hh>

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
    /// on a stable but slower storage device such as an hard disk and
    /// the hole which represents the only storage space.
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
      // globals
      //
      static core::Time		Delays[hole::Block::Families];

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

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/depot/Cache.hh>
#include <etoile/depot/Record.hh>
#include <etoile/depot/House.hh>

#endif
