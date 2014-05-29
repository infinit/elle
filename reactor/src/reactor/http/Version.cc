#include <ostream>

#include <elle/assert.hh>

#include <reactor/http/Version.hh>

namespace reactor
{
  namespace http
  {
    std::ostream&
    operator <<(std::ostream& output,
                Version version)
    {
      switch (version)
      {
        case Version::v10:
          output << "HTTP/1.0";
          break;
        case Version::v11:
          output << "HTTP/1.1";
          break;
        default:
          elle::unreachable();
      }
      return output;
    }
  }
}
