//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Component.hh
//
// created       julien quintard   [mon aug 10 14:14:40 2009]
// updated       julien quintard   [mon aug 10 14:25:25 2009]
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
	ComponentUnknown,
	ComponentDirectory,
	ComponentFile,
	ComponentLink
      };

  }
}

#endif
