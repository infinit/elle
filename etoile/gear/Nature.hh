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
// updated       julien quintard   [thu jun 23 16:18:21 2011]
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
	NatureObject,
	NatureFile,
	NatureDirectory,
	NatureLink
      };

  }
}

#endif
