//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/neutron/Component.hh
//
// created       julien quintard   [thu may 12 12:58:32 2011]
// updated       julien quintard   [thu may 12 13:00:13 2011]
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

	ComponentUser,
	ComponentGroup,
	ComponentObject,
	ComponentContents,
	ComponentData,
	ComponentCatalog,
	ComponentReference,
	ComponentAccess,

	Components = ComponentAccess + 1
      };

  }
}

#endif
