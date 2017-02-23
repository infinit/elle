#include <elle/reactor/http/url.hh>

#include <curl/curl.h>

namespace elle
{
  namespace reactor
  {
    namespace http
    {
      std::string
      url_encode(std::string const& str)
      {
        char* escaped_string = curl_escape(str.c_str(), str.size());
        std::string res(escaped_string);
        curl_free(escaped_string);
        return res;
      }

      std::string
      url_decode(std::string const& str)
      {
        char* escaped_string = curl_unescape(str.c_str(), str.size());
        std::string res(escaped_string);
        curl_free(escaped_string);
        return res;
      }
    }
  }
}
