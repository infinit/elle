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
          return output << "HTTP/1.0";
        case Version::v11:
          return output << "HTTP/1.1";
        case Version::v20:
          return output << "HTTP/2.0";
      }
      elle::unreachable();
    }

    namespace version
    {
      Version
      from_string(std::string const& name)
      {
        if (name == "HTTP/1.0") return Version::v10;
        else if (name == "HTTP/1.1") return Version::v11;
        else if (name == "HTTP/2.0") return Version::v20;
        else throw elle::Exception(
          elle::sprintf("unknown version %s", name));
      }
    }
  }
}
