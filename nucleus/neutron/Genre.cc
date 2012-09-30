#include <nucleus/neutron/Genre.hh>
#include <nucleus/Exception.hh>

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
        case Genre::file:
          {
            stream << "file";
            break;
          }
        case Genre::directory:
          {
            stream << "directory";
            break;
          }
        case Genre::link:
          {
            stream << "link";
            break;
          }
        default:
          {
            throw Exception("unknown genre '%s'", genre);
          }
        }

      return (stream);
    }

  }
}
