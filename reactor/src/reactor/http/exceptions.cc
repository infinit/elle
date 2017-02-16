#include <curl/curl.h>

#include <elle/printf.hh>

#include <reactor/http/exceptions.hh>

namespace reactor
{
  namespace http
  {
    RequestError::RequestError(std::string const& url,
                               std::string const& error)
      : Super(elle::sprintf("error requesting %s: %s", url, error))
      , _url(url)
      , _error(error)
    {}

    EmptyResponse::EmptyResponse(std::string const& url)
      : Super(curl_easy_strerror(CURLE_GOT_NOTHING), url)
    {}

    Timeout::Timeout(std::string const& url,
                     reactor::Duration const& timeout)
      : Super(url, elle::sprintf("timeout with delay %s", timeout))
      , _timeout(timeout)
    {}

    ResolutionFailure::ResolutionFailure(std::string const& url)
      : Super(url, "resolution failure")
    {}
  }
}
