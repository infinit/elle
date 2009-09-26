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
// updated       julien quintard   [sat sep 12 00:06:04 2009]
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

      template <typename T>
      static Status	Get(const hole::Address&,
			    T&);
      template <typename T>
      static Status	Put(const hole::Address&,
			    const T&);

      static Status	Expiration(const core::ContentHashBlock&,
				   core::Time&);
      static Status	Expiration(const core::PublicKeyBlock&,
				   core::Time&);
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <etoile/depot/Depot.hxx>

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/depot/Cache.hh>

#endif
