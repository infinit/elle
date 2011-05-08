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
// updated       julien quintard   [sun may  8 12:25:38 2011]
//

#ifndef ETOILE_DEPOT_DEPOT_HH
#define ETOILE_DEPOT_DEPOT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/hole/Hole.hh>

namespace etoile
{
  namespace depot
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class abstracts the storage layer into three units: the cache
    /// which keeps blocks in main memory, the reserve which keeps block
    /// on a stable but slower storage device such as on a hard disk and
    /// the hole which represents the online storage space.
    ///
    /// note that blocks are passed as pointers so that no allocation nor
    /// copies are required, leading to better performance.
    ///
    /// furthermore, the depot---hence the underlying cache, reserve and
    /// hole---are expecting to take blocks allocated through the new
    /// operator. likewise, the Get() method returns an already
    /// new-operator-allocated block.
    ///
    class Depot
    {
    public:
      //
      // static methods
      //
      static elle::Status	Initialize();
      static elle::Status	Clean();

      //
      // templates
      //

      // XXX a mettre dans Depot.hxx

      ///
      /// this method has been introduced because the C++ typing system
      /// seems unable to implicitly cast an nucleus::Object* for instance
      /// to a hole::Block*& or to const-equivalents.
      ///
      template <typename T>
      static elle::Status	Put(const nucleus::Network&	network,
				    const nucleus::Address&	address,
				    T*				block)
      {
	enter();

	// store in the hole.
	if (hole::Hole::Put(network,
			    address,
			    (nucleus::Block*)block) == elle::StatusError)
	  escape("unable to put the block in the hole");

	leave();
      }

      template <typename T>
      static elle::Status	Get(const nucleus::Network&	network,
				    const nucleus::Address&	address,
				    T*&				block)
      {
	enter();

	// XXX look in the cache etc.

	// since the block has not been found in the cache, allocate
	// one which will retrieved from the hole.
	block = new T;

	// finally, look in the hole.
	if (hole::Hole::Get(network,
			    address,
			    (nucleus::Block*&)block) == elle::StatusOk)
	  leave();

	escape("unable to locate the block");
      }

      static elle::Status	Erase(const nucleus::Network&	network,
				      const nucleus::Address&	address)
      {
	enter();

	// XXX look in the cache etc.

	// finally, erase the block from the hole.
	if (hole::Hole::Erase(network,
			      address) == elle::StatusOk)
	  leave();

	escape("unable to erase the block");
      }
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/depot/Cell.hh>
#include <etoile/depot/Location.hh>
#include <etoile/depot/Record.hh>
#include <etoile/depot/Repository.hh>
#include <etoile/depot/Unit.hh>

#endif
