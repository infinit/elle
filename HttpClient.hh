#ifndef ELLE_HTTPCLIENT_HH
# define ELLE_HTTPCLIENT_HH

# include <elle/Buffer.hh>
# include <elle/format/json.hh>

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

  //- Exception ---------------------------------------------------------------

  /// Exception thrown by HttpClient methods
  class HTTPException
    : public std::runtime_error
  {
  public:
    ResponseCode const code;

  public:
    HTTPException(ResponseCode code, std::string const& message);
  }; // ! HTTPException

  class HttpClient;

  //- Request -----------------------------------------------------------------
  class Request
  {
  private:
    struct Impl;
    std::unique_ptr<Impl> _this;

  public:
    Request(HttpClient& client,
            std::string const& method,
            std::string const& url);
    Request(Request&& other);
    ~Request();

  public:
    std::string const& method() const;
    std::string const& url() const;
    std::string body_string() const;
    std::string headers_string() const;
    std::string const& content_type() const;

  public:
    bool
    has_header(std::string const& key) const;

    std::string const&
    header(std::string const& key);

    Request&
    header(std::string const& key, std::string const& value);

    Request&
    post_field(std::string const& key,
               std::string const& value);

    Request&
    content_type(std::string const& str);

    void fire();

    std::stringstream&
    response();

  };

  //- HttpClient --------------------------------------------------------------
  class HttpClient
  {
  private:
    struct Impl;
    std::unique_ptr<Impl> _impl;
    std::string _token;

  public:
    HttpClient(std::string const& server,
               uint16_t port,
               bool check_errors);
    ~HttpClient();

    void
    token(std::string const& token) { _token = token; }

    std::string const&
    token() const { return _token; }

  public:
    template<typename T>
    T get(std::string const& url);

    template<typename T>
    T post(std::string const& url, elle::format::json::Object const& req);

    elle::Buffer
    get_buffer(std::string const& url);

    Request
    request(std::string const& method, std::string const& url);

    void fire(Request& request);

  private:
    ///XXX Remove this
    void
    _request(std::string const& url,
             std::string const& method,
             std::string const& body,
             std::stringstream& response);
  };
}

#endif
