#ifndef NUCLEUS_NEUTRON_GENRE_HH
# define NUCLEUS_NEUTRON_GENRE_HH

#include <elle/types.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- types -----------------------------------------------------------
//

    ///
    /// the genre defines the type of object: file, directory or link.
    ///
    typedef elle::Natural8              Genre;

//
// ---------- constants -------------------------------------------------------
//

    ///
    /// default genre values.
    ///
    const Genre                 GenreUnknown = 0;
    const Genre                 GenreDirectory = 1;
    const Genre                 GenreFile = 2;
    const Genre                 GenreLink = 3;

  }
}

#endif
