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
// updated       julien quintard   [fri may 13 10:26:49 2011]
//

#ifndef ETOILE_DEPOT_DEPOT_HH
#define ETOILE_DEPOT_DEPOT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/depot/Hole.hh>

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
      // methods
      //
      static elle::Status	Put(const nucleus::Network&,
				    const nucleus::Address&,
				    const nucleus::Block&);
      static elle::Status	Get(const nucleus::Network&,
				    const nucleus::Address&,
				    nucleus::Block&);
      static elle::Status	Erase(const nucleus::Network&,
				      const nucleus::Address&);

    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/depot/Cell.hh>
#include <etoile/depot/Location.hh>
#include <etoile/depot/Hole.hh>
#include <etoile/depot/Record.hh>
#include <etoile/depot/Repository.hh>
#include <etoile/depot/Unit.hh>

#endif
