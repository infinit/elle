#ifndef NUCLEUS_NEUTRON_GENRE_HH
# define NUCLEUS_NEUTRON_GENRE_HH

# include <elle/types.hh>
# include <iosfwd>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- enumerations ----------------------------------------------------
//

    /// Defines the nature of a file system object being either a file,
    /// directory or link (i.e symbolic link in UNIX terms).
    enum class Genre
    {
      file,
      directory,
      link
    };

    std::ostream&
    operator <<(std::ostream& stream,
                Genre const genre);

  }
}

#endif
