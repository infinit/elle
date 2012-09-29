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
        case GenreUnknown:
          {
            stream << "unknown";
            break;
          }
        case GenreDirectory:
          {
            stream << "directory";
            break;
          }
        case GenreFile:
          {
            stream << "file";
            break;
          }
        case GenreLink:
          {
            stream << "link";
            break;
          }
        }
    }

  }
}
