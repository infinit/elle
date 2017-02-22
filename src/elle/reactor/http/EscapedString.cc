#include <curl/curl.h>

#include <elle/log.hh>
#include <elle/reactor/http/EscapedString.hh>

ELLE_LOG_COMPONENT("elle.reactor.http.EscapedString");

namespace elle
{
  namespace reactor
  {
    namespace http
    {
      EscapedString::EscapedString(std::string const& str)
      {
        ELLE_DEBUG("%s: given string: %s", *this, str);
        this->_string = this->_curl_format(str);
        ELLE_DEBUG("%s: escaped string: %s", *this, this->_string);
      }

      std::string
      EscapedString::_curl_format(std::string const& str)
      {
        // curl_escape is deprecated but it doesn't require a curl handle.
        char* escaped_string = curl_escape(str.c_str(), str.size());
        std::string res(escaped_string);
        curl_free(escaped_string);
        return res;
      }

      void
      EscapedString::print(std::ostream& stream) const
      {
        stream << this->_string;
      }
    }
  }
}
