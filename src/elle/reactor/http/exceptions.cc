#include <curl/curl.h>

#include <elle/print.hh>
#include <elle/reactor/http/exceptions.hh>

namespace elle
{
  namespace reactor
  {
    namespace http
    {
      RequestError::RequestError(std::string url, std::string error)
        : Super{elle::print("error requesting %s: %s", url, error)}
        , _url{std::move(url)}
        , _error{std::move(error)}
      {}

      EmptyResponse::EmptyResponse(std::string url)
        : Super{curl_easy_strerror(CURLE_GOT_NOTHING), std::move(url)}
      {}

      Timeout::Timeout(std::string url,
                       reactor::Duration const& timeout)
        : Super{std::move(url), elle::print("timeout with delay %s", timeout)}
        , _timeout(timeout)
      {}

      ResolutionFailure::ResolutionFailure(std::string url)
        : Super(std::move(url), "resolution failure")
      {}
    }
  }
}
