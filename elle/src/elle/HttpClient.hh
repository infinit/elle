#ifndef ELLE_HTTPCLIENT_HH
# define ELLE_HTTPCLIENT_HH

# include <elle/Buffer.hh>
# include <elle/format/json.hh>
# include <elle/attribute.hh>

namespace elle
{
  namespace http
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
    class Exception
      : public elle::Exception
    {
    public:
      ResponseCode const code;

    public:
      Exception(ResponseCode code, std::string const& message = "");
      bool operator ==(Exception const& e);
    }; // ! HTTPException
  }

  class HTTPClient;

  //- Request -----------------------------------------------------------------
  class Request
  {
    /*-------------.
    | Construction |
    `-------------*/
  public:
    /// Initialize request like METHOD on server/url
    Request(HTTPClient& client,
            std::string const& method,
            std::string const& url);
    /// Initialize Request by copy.
    Request(Request&& other);
    /// Destroy instance.
    ~Request();

  public:
    std::string const& method() const;

    std::string url() const;

    bool has_body() const;
    std::unique_ptr<std::istream> body() const;
    Request& body(std::unique_ptr<std::istream>&& input);

    std::string body_string() const;
    Request& body_string(std::string const& str);

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
    user_agent(std::string const& str);

    Request&
    post_field(std::string const& key,
               std::string const& value);

    Request&
    parameter(std::string const& key,
              std::string const& value);

    Request&
    content_type(std::string const& str);

    Request& fire();

    std::stringstream&
    response();

  private:
    struct Impl;
    std::unique_ptr<Impl> _this;
  };

  //- HTTPClient --------------------------------------------------------------
  class HTTPClient
  {
    /*-------------.
    | Construction |
    `-------------*/
  public:
    /// Initialize the client to server:port.
    HTTPClient(std::string const& server,
               uint16_t port,
               std::string const& user_agent);

    /// Destroy client.
    ~HTTPClient();

  public:
    // /// Use a get method to server.
    // /// T must be serializable by elle::serialize::InputJSONArchive
    // template<typename T>
    // T get(std::string const& url);

    // /// Use a post method to server.
    // /// T must be serializable by elle::serialize::InputJSONArchive
    // template<typename T>
    // T post(std::string const& url, elle::format::json::Object const& req);

    /// Use a put method to server.
    bool
    put(std::string const& url,
        elle::format::json::Object const& req);

    /// Use a simple get to url.
    elle::Buffer
    get_buffer(std::string const& url);

    /// Construct a request bind to this server.
    Request
    request(std::string const& method, std::string const& url);

    /// Fire a prebuilt request.
    void fire(Request& request);

  private:
    // ///XXX Remove this
    // void
    // _request(std::string const& url,
    //          std::string const& method,
    //          std::string const& body,
    //          std::stringstream& response);

    /*-----------.
    | Attributes |
    `-----------*/
  private:
    /// Forward to avoid including asio.
    struct Impl;
    std::unique_ptr<Impl> _impl;

    ELLE_ATTRIBUTE_RW(std::string, token);
  };
}

#endif
