#include <nucleus/neutron/Genre.hh>

#include <elle/printf.hh>

#include <iostream>
#include <stdexcept>

namespace nucleus
{
  namespace neutron
  {

    std::ostream&
    operator <<(std::ostream& stream,
                Genre const genre)
    {
      switch (genre)
        {
        case GenreFile:
          {
            stream << "file";
            break;
          }
        case GenreDirectory:
          {
            stream << "directory";
            break;
          }
        case GenreLink:
          {
            stream << "link";
            break;
          }
        default
          {
            throw Exception("unknown genre '%s'", genre);
          }
        }
    }

  }
}
