#include <ostream>

#include <elle/assert.hh>
#include <elle/err.hh>
#include <elle/printf.hh>

#include <reactor/http/Method.hh>

namespace reactor
{
  namespace http
  {
    namespace method
    {
      Method
      from_string(std::string const& name)
      {
        if (name == "DELETE") return Method::DELETE;
        if (name == "GET") return Method::GET;
        if (name == "POST") return Method::POST;
        if (name == "PUT") return Method::PUT;
        elle::err("unknown method %s", name);
      }
    }

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
