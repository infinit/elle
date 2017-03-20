#include <ostream>

#include <elle/assert.hh>
#include <elle/err.hh>
#include <elle/printf.hh>
#include <elle/reactor/http/Method.hh>

namespace elle
{
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
          else if (name == "GET") return Method::GET;
          else if (name == "HEAD") return Method::HEAD;
          else if (name == "OPTIONS") return Method::OPTIONS;
          else if (name == "POST") return Method::POST;
          else if (name == "PUT") return Method::PUT;
          else elle::err("unknown method %s", name);
        }
      }

      std::ostream&
      operator <<(std::ostream& output,
                  Method method)
      {
        switch (method)
        {
          case Method::DELETE:
            return output << "DELETE";
          case Method::GET:
            return output << "GET";
          case Method::HEAD:
            return output << "HEAD";
          case Method::OPTIONS:
            return output << "OPTIONS";
          case Method::POST:
            return output << "POST";
          case Method::PUT:
            return output << "PUT";
        }
        elle::unreachable();
      }
    }
  }
}
