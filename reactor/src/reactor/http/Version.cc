#include <ostream>

#include <elle/assert.hh>
#include <elle/Exception.hh>

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
        case Version::v20:
          output << "HTTP/2.0";
          break;
        default:
          elle::unreachable();
      }
      return output;
    }

    namespace version
    {
      Version
      from_string(std::string const& name)
      {
        if (name == "HTTP/1.0") return Version::v10;
        if (name == "HTTP/1.1") return Version::v11;
        if (name == "HTTP/2.0") return Version::v20;

        throw elle::Exception(
          elle::sprintf("unknown version %s", name));
      }
    }
  }
}
