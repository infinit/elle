//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/kernel/Component.hh
//
// created       julien quintard   [fri aug 14 23:11:50 2009]
// updated       julien quintard   [fri apr  2 02:43:29 2010]
//

#ifndef ETOILE_KERNEL_COMPONENT_HH
#define ETOILE_KERNEL_COMPONENT_HH

namespace etoile
{
  namespace kernel
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
