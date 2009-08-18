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
// updated       julien quintard   [sat aug 15 00:16:21 2009]
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

    enum Component
      {
	ComponentUnknown = 0x0,
	ComponentObject = 0x1,
	ComponentFile = ComponentObject + 0x2,
	ComponentDirectory = ComponentObject + 0x4,
	ComponentLink = 0x8,
	ComponentUser = 0x10,
	ComponentGroup = 0x20
      };

  }
}

#endif
