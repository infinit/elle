#include <curly/exceptions.hh>

#include <elle/printf.hh>

namespace curly
{
  RequestError::RequestError(CURLcode error,
                             std::string const& url,
                             std::string const& message):
    Super(elle::sprintf("error requesting %s: %s: %s",
                        url, curl_easy_strerror(error), message)),
    _url(url),
    _error(error)
  {}

  RequestError::RequestError(CURLcode error,
                             std::string const& url):
    Super(elle::sprintf("error requesting %s: %s",
                        url, curl_easy_strerror(error))),
    _url(url),
    _error(error)
  {}

  EmptyResponse::EmptyResponse(std::string const& url):
    Super(CURLE_GOT_NOTHING, url)
  {}
}
