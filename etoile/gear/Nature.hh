#ifndef ETOILE_GEAR_NATURE_HH
# define ETOILE_GEAR_NATURE_HH

namespace etoile
{
  namespace gear
  {

    ///
    /// this enumeration defines the nature of a context.
    ///
    enum Nature
      {
        NatureUnknown,

        NatureObject,
        NatureFile,
        NatureDirectory,
        NatureLink,
        NatureGroup
      };

  }
}

#endif
