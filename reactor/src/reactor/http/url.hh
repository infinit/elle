#ifndef REACTOR_HTTP_URL_HH
# define REACTOR_HTTP_URL_HH

#include <string>

namespace reactor
{
  namespace http
  {
    std::string url_encode(std::string const& str);
    std::string url_decode(std::string const& str);
  }
}

#endif