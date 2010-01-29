//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/depot/Location.hh
//
// created       julien quintard   [wed jan 27 12:31:11 2010]
// updated       julien quintard   [thu jan 28 23:08:46 2010]
//

#ifndef ETOILE_DEPOT_LOCATION_HH
#define ETOILE_DEPOT_LOCATION_HH

namespace etoile
{
  namespace depot
  {

//
// ---------- enumerations ----------------------------------------------------
//

    ///
    /// this defines the location of a block, either in main memory i.e cache
    /// or on a slower but larger medium, i.e reserve.
    ///
    enum Location
      {
	LocationUnknown,
	LocationCache,
	LocationReserve
      };

  }
}

#endif
