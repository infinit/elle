//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/hole/Component.hh
//
// created       julien quintard   [fri aug 14 23:11:50 2009]
// updated       julien quintard   [thu apr 22 17:21:51 2010]
//

#ifndef ETOILE_HOLE_COMPONENT_HH
#define ETOILE_HOLE_COMPONENT_HH

namespace etoile
{
  namespace hole
  {

//
// ---------- enumerations ----------------------------------------------------
//

    ///
    /// the component represents the different base type of blocks that
    /// are stored in the system.
    ///
    enum Component
      {
	ComponentUnknown = 0x00,

	ComponentObject = 0x01,
	ComponentData = 0x02,
	ComponentCatalog = 0x04,
	ComponentReference = 0x08,
	ComponentAccess = 0x10,
	ComponentUser = 0x20,
	ComponentGroup = 0x40
      };

  }
}

#endif
