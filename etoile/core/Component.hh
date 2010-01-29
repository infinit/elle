//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Component.hh
//
// created       julien quintard   [fri aug 14 23:11:50 2009]
// updated       julien quintard   [thu jan 28 21:42:15 2010]
//

#ifndef ETOILE_CORE_COMPONENT_HH
#define ETOILE_CORE_COMPONENT_HH

namespace etoile
{
  namespace core
  {

//
// ---------- enumerations ----------------------------------------------------
//

    ///
    /// XXX
    ///
    enum Component
      {
	ComponentUnknown = 0x0,
	ComponentObject = 0x1,
	ComponentData = 0x2,
	ComponentCatalog = 0x4,
	ComponentReference = 0x8,
	ComponentUser = 0x10,
	ComponentGroup = 0x20
      };

  }
}

#endif
