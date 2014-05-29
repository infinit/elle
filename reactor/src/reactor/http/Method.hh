#ifndef REACTOR_HTTP_METHOD_HH
# define REACTOR_HTTP_METHOD_HH

# include <iosfwd>

namespace reactor
{
  namespace http
  {
    /// HTTP methods used to perform resquests.
    enum class Method
    {
# ifdef DELETE
#  undef DELETE
# endif
      DELETE,
      GET,
      POST,
      PUT,
    };

    /// Pretty print a HTTP method.
    std::ostream&
    operator <<(std::ostream& output,
                Method method);
  }
}
#endif
