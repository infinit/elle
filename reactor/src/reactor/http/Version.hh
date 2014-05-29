#ifndef REACTOR_HTTP_VERSION_HH
# define REACTOR_HTTP_VERSION_HH

# include <iosfwd>

namespace reactor
{
  namespace http
  {
    enum class Version
    {
      v10,
      v11,
    };

    std::ostream&
    operator <<(std::ostream& output,
                Version method);
  }
}

#endif
