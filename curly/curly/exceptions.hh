#ifndef ELLE_CURLY_EXCEPTIONS_HH
# define ELLE_CURLY_EXCEPTIONS_HH

# include <curl/curl.h>

# include <elle/Exception.hh>

namespace curly
{
  class RequestError:
    public elle::Exception
  {
  public:
    typedef elle::Exception Super;
    RequestError(CURLcode error,
                 std::string const& url,
                 std::string const& message);
    RequestError(CURLcode error,
                 std::string const& url);
  private:
    ELLE_ATTRIBUTE_R(std::string, url);
    ELLE_ATTRIBUTE_R(CURLcode, error);
  };

  class EmptyResponse:
    public RequestError
  {
  public:
    typedef RequestError Super;
    EmptyResponse(std::string const& url);
  };
}

#endif
