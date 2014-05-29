#include <ostream>

#include <elle/assert.hh>

#include <reactor/http/Method.hh>

namespace reactor
{
  namespace http
  {
    std::ostream&
    operator <<(std::ostream& output,
                Method method)
    {
      switch (method)
      {
        case Method::DELETE:
          output << "DELETE";
          break;
        case Method::GET:
          output << "GET";
          break;
        case Method::POST:
          output << "POST";
          break;
        case Method::PUT:
          output << "PUT";
          break;
        default:
          elle::unreachable();
      }
      return output;
    }
  }
}
