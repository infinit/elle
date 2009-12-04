//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Genre.hh
//
// created       julien quintard   [mon aug 10 14:14:40 2009]
// updated       julien quintard   [tue dec  1 02:21:42 2009]
//

#ifndef ETOILE_CORE_GENRE_HH
#define ETOILE_CORE_GENRE_HH

namespace etoile
{
  namespace core
  {

//
// ---------- enumerations ----------------------------------------------------
//

    ///
    /// the genre defines the type of object: file, directory or link.
    ///
    enum Genre
      {
	GenreUnknown,
	GenreDirectory,
	GenreFile,
	GenreLink
      };

  }
}

#endif
