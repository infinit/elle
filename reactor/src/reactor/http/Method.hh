#pragma once

#include <iosfwd>

namespace reactor
{
  namespace http
  {
    /// HTTP methods used to perform resquests.
    enum class Method
    {
#ifdef DELETE
# undef DELETE
#endif
      DELETE,
      GET,
      POST,
      PUT,
    };

    namespace method
    {
      Method
      from_string(std::string const&);
    }

    /// Pretty print a HTTP method.
    std::ostream&
    operator <<(std::ostream& output,
                Method method);
  }
}
