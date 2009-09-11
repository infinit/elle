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
// updated       julien quintard   [fri sep 11 01:25:12 2009]
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
    /// XXX
    ///
    class Depot
    {
    public:
      //
      // constants
      //
      struct		Delays
      {
	static core::Time		PublicKeyBlock;
	// XXX more blocks
      };

      //
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();

      static Status	Get(const hole::Address&,
			    hole::Block&);
      static Status	Put(const hole::Address&,
			    const hole::Block&,
			    const core::Time&);

      static Status	Put(const hole::Address&,
			    const core::ContentHashBlock&);
      static Status	Put(const hole::Address&,
			    const core::PublicKeyBlock&);
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/depot/Cache.hh>

#endif
