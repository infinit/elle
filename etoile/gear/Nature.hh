//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [fri jun  3 11:32:52 2011]
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
