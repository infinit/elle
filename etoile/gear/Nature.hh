//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Nature.hh
//
// created       julien quintard   [fri jun  3 11:32:52 2011]
// updated       julien quintard   [fri jul 29 12:30:15 2011]
//

#ifndef ETOILE_GEAR_NATURE_HH
#define ETOILE_GEAR_NATURE_HH

namespace etoile
{
  namespace gear
  {

//
// ---------- enumeartions ----------------------------------------------------
//

    ///
    /// this enumeration defines the nature of a context.
    ///
    enum Nature
      {
	NatureUnknown,

	NatureObject,
	NatureFile,
	NatureDirectory,
	NatureLink
      };

  }
}

#endif
