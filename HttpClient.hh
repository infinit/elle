#ifndef ELLE_HTTPCLIENT_HH
# define ELLE_HTTPCLIENT_HH

# include <elle/format/json.hh>
# include <elle/format/json/fwd.hh>

namespace elle
{
  // Error values.
  enum class ResponseCode : int
  {
    // Success
    ok = 200,
    accepted = 202,

    // Client error
    error = 400,
    bad_request = 400,
    forbidden = 403,
    not_found = 404,
    im_a_teapot = 418,

    // Server error
    internal_server_error = 500,
    bad_content = 601,
    unknown_error = 666
  };

  /// Exception thrown by HttpClient methods
  class HTTPException
    : public std::runtime_error
  {
  public:
    ResponseCode const code;

  public:
    HTTPException(ResponseCode code, std::string const& message);
  }; // ! HTTPException

  class HttpClient
  {
  private:
    struct Impl;
    Impl* _impl;
    std::string _token;

  public:
    HttpClient(std::string const& server,
               uint16_t port,
               bool check_errors);

    void
    token(std::string const& token) { _token = token; }

    std::string const&
    token() const { return _token; }

  public:
    template<typename T>
    T get(std::string const& url);

    template<typename T>
    T post(std::string const& url, elle::format::json::Object const& req);

  private:
    void
    _request(std::string const& url,
             std::string const& method,
             std::string const& body,
             std::stringstream& response);
  };
}

#endif
