#ifndef NUCLEUS_NEUTRON_GENRE_HH
# define NUCLEUS_NEUTRON_GENRE_HH

# include <elle/types.hh>
# include <iosfwd>

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
    /// XXX[change in enum class]
    typedef elle::Natural8              Genre;

//
// ---------- constants -------------------------------------------------------
//

    const Genre                 GenreUnknown = 0;
    const Genre                 GenreDirectory = 1;
    const Genre                 GenreFile = 2;
    const Genre                 GenreLink = 3;

    std::ostream&
    operator <<(std::ostream& stream,
                Genre const genre);

  }
}

#endif
