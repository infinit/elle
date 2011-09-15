//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [thu may 12 12:58:32 2011]
//

#ifndef NUCLEUS_NEUTRON_COMPONENT_HH
#define NUCLEUS_NEUTRON_COMPONENT_HH

namespace nucleus
{
  namespace neutron
  {

//
// ---------- enumerations ----------------------------------------------------
//

    ///
    /// this enumeration defines the block's logical component.
    ///
    enum Component
      {
	ComponentUnknown = 0,

	ComponentObject,
	ComponentContents,
	ComponentData,
	ComponentCatalog,
	ComponentReference,
	ComponentAccess,
	ComponentUser,
	ComponentGroup,

	Components = ComponentAccess + 1
      };

  }
}

#endif
