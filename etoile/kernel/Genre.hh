//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/kernel/Genre.hh
//
// created       julien quintard   [mon aug 10 14:14:40 2009]
// updated       julien quintard   [wed mar 31 21:32:26 2010]
//

#ifndef ETOILE_KERNEL_GENRE_HH
#define ETOILE_KERNEL_GENRE_HH

namespace etoile
{
  namespace kernel
  {

//
// ---------- types -----------------------------------------------------------
//

    ///
    /// the genre defines the type of object: file, directory or link.
    ///
    typedef Natural8		Genre;

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
