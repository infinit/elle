//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/neutron/Genre.hh
//
// created       julien quintard   [mon aug 10 14:14:40 2009]
// updated       julien quintard   [wed may  4 23:20:42 2011]
//

#ifndef NUCLEUS_NEUTRON_GENRE_HH
#define NUCLEUS_NEUTRON_GENRE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

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
    typedef elle::Natural8		Genre;

//
// ---------- constants -------------------------------------------------------
//

    ///
    /// default genre values.
    ///
    const Genre			GenreUnknown = 0;
    const Genre			GenreDirectory = 1;
    const Genre			GenreFile = 2;
    const Genre			GenreLink = 3;

  }
}

#endif
