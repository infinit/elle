#include "HttpClient.hxx"

#include <elle/log.hh>
#include <elle/print.hh>
#include <elle/serialize/JSONArchive.hh>

# define CRLF "\r\n"

namespace elle
{
  //- Exception ---------------------------------------------------------------
  HTTPException::HTTPException(ResponseCode code, std::string const& message)
    : std::runtime_error(message)
    , code(code)
  {}


  //- Request -----------------------------------------------------------------

  struct Request::Impl
  {
    HttpClient&                                   client;
    std::string                                   method;
    std::string                                   url;
    std::string                                   content_type;
    std::unordered_map<std::string, std::string>  headers;
    std::unordered_map<std::string, std::string>  post_fields;
    std::stringstream                             response;

    Impl(HttpClient& client,
         std::string const& method,
         std::string const& url)
      : client(client)
      , method{method}
      , url{url}
    {}
  };

  Request::Request(HttpClient& client,
                   std::string const& method,
                   std::string const& url)
    : _this{new Impl{client, method, url}}
  {}

  Request::Request(Request&& other)
    : _this{std::move(other._this)}
  {}

  Request::~Request()
  {}

  // properties

  std::string
  Request::body_string() const
  {
    std::string body;
    bool first = true;
    for (auto const& pair: _this->post_fields)
      {
        if (!first)
          body += "&";
        body += pair.first + "=" + pair.second; //XXX must be encoded.
      }
    return body;
  }

  std::string const&
  Request::method() const { return _this->method; }

  std::string const&
  Request::url() const { return _this->url; }

  std::string const&
  Request::content_type() const { return _this->content_type; }

  std::string
  Request::headers_string() const
  {
    std::string headers;
    for (auto const& pair: _this->headers)
        headers += pair.first + ": " + pair.second + CRLF;
    return headers;
  }

  // content type, headers and body

  bool
  Request::has_header(std::string const& key) const
  {
    return (_this->headers.find(key) != _this->headers.end());
  }

  std::string const&
  Request::header(std::string const& key)
  {
    auto it = _this->headers.find(key);
    if (it == _this->headers.end())
      throw std::runtime_error{"Cannot find header '" + key + "'"};
    return it->second;
  }

  Request&
  Request::header(std::string const& key,
                  std::string const& value)
  {
    _this->headers[key] = value;
    return *this;
  }

  Request&
  Request::content_type(std::string const& str)
  {
    _this->content_type = str;
    return *this;
  }

  Request&
  Request::post_field(std::string const& key,
                      std::string const& value)
  {
    _this->post_fields[key] = value;
    return *this;
  }

  void
  Request::fire()
  {
    _this->client.fire(*this);
  }

  std::stringstream&
  Request::response()
  {
    return _this->response;
  }

  //- HttpClient --------------------------------------------------------------
  HttpClient::HttpClient(std::string const& server,
                         uint16_t port,
                         bool check_errors)
    : _impl{new Impl{server, port, check_errors}}
    , _token{}
  {}

  HttpClient::~HttpClient()
  {}

  elle::Buffer
  HttpClient::get_buffer(std::string const& url)
  {
    // XXX not optimized (mulptiple copies).
    std::stringstream ss;
    this->_request(url, "GET", "", ss);
    std::string res = ss.str();
    return elle::Buffer{(elle::Byte*) res.data(), res.size()};
  }

  Request
  HttpClient::request(std::string const& method,
                      std::string const& url)
  {
    return Request{*this, method, url};
  }

  void
  HttpClient::fire(Request& request)
  {

    std::string uri = _impl->server + request.url();

    namespace ip = boost::asio::ip;

    ip::tcp::socket socket(_impl->io_service);

    ip::tcp::resolver resolver(_impl->io_service);
    ip::tcp::resolver::query query(_impl->server, elle::sprint(_impl->port));
    ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    //XXX which way is better ?
    //boost::asio::connect(socket, endpoint_iterator);
    socket.connect(*endpoint_iterator);

    std::string body = request.body_string();
    {
      boost::asio::streambuf request_buf;
      std::ostream request_stream(&request_buf);
      request_stream << request.method() << ' ' << request.url() << " HTTP/1.0" CRLF
                     << "Host: " << _impl->server << CRLF
                     << "User-Agent: MetaClient" CRLF
                     << "Connection: close" CRLF
                     << request.headers_string();

      if (body.size())
        request_stream << "Content-Length: " << elle::sprint(body.size()) << CRLF
                       << "Content-Type: " << request.content_type() << CRLF CRLF
                       << body;
      else
        request_stream << CRLF;

      // Send the request.
      boost::asio::write(socket, request_buf);
    }

    // Read the response status line. The response streambuf will automatically
    // grow to accommodate the entire line. The growth may be limited by passing
    // a maximum size to the streambuf constructor.
    boost::asio::streambuf response;
    boost::asio::read_until(socket, response, "\r\n");

    // Check that response is OK.
    std::istream response_stream(&response);
    std::string http_version;
    response_stream >> http_version;
    unsigned int status_code;
    response_stream >> status_code;
    std::string status_message;
    std::getline(response_stream, status_message);
    if (!response_stream || http_version.substr(0, 5) != "HTTP/")
      throw std::runtime_error("Invalid response");
    if (status_code != 200)
      {
        throw std::runtime_error(
          "Cannot " + request.method() + " '" + body + "' " + uri +
          " returned HTTP code " + elle::sprint(status_code) +
          ": " + status_message
          );
      }

    // Read the response headers, which are terminated by a blank line.
    boost::asio::read_until(socket, response, "\r\n\r\n");

    // Process the response headers.
    std::string header;
    while (std::getline(response_stream, header) && header != "\r")
      {/* consume header lines */}

    // Write whatever content we already have to output.
    if (response.size() > 0)
      request.response() << &response;

    // Read until EOF, writing data to output as we go.
    boost::system::error_code error;
    while (boost::asio::read(socket,
                             response,
                             boost::asio::transfer_at_least(1), error))
      {
        request.response() << &response;
        if (error && error != boost::asio::error::eof)
          throw boost::system::system_error(error);
      }
  }

  /// XXX Remove this
  void
  HttpClient::_request(std::string const& url,
                       std::string const& method,
                       std::string const& body,
                       std::stringstream& response_)
  {
    std::string uri = _impl->server + url;

    namespace ip = boost::asio::ip;

    ip::tcp::socket socket(_impl->io_service);

    ip::tcp::resolver resolver(_impl->io_service);
    ip::tcp::resolver::query query(_impl->server, elle::sprint(_impl->port));
    ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    //XXX which way is better ?
    //boost::asio::connect(socket, endpoint_iterator);
    socket.connect(*endpoint_iterator);

    {
      boost::asio::streambuf request;
      std::ostream request_stream(&request);
      request_stream << method << ' ' << url << " HTTP/1.0" CRLF
                     << "Host: " << _impl->server << CRLF
                     << "User-Agent: MetaClient" CRLF
                     << "Connection: close" CRLF;
      if (_token.size())
        {
          request_stream << "Authorization: " << _token << CRLF;
        }
      if (body.size())
        request_stream << "Content-Length: " << elle::sprint(body.size()) << CRLF
                       << "Content-Type: application/json" << CRLF CRLF
                       << body;
      else
        request_stream << CRLF;

      // Send the request.
      boost::asio::write(socket, request);
    }

    // Read the response status line. The response streambuf will automatically
    // grow to accommodate the entire line. The growth may be limited by passing
    // a maximum size to the streambuf constructor.
    boost::asio::streambuf response;
    boost::asio::read_until(socket, response, "\r\n");

    // Check that response is OK.
    std::istream response_stream(&response);
    std::string http_version;
    response_stream >> http_version;
    unsigned int status_code;
    response_stream >> status_code;
    std::string status_message;
    std::getline(response_stream, status_message);
    if (!response_stream || http_version.substr(0, 5) != "HTTP/")
      throw std::runtime_error("Invalid response");
    if (status_code != 200)
      {
        throw std::runtime_error(
          "Cannot " + method + " '" + body + "' " + uri +
          " returned HTTP code " + elle::sprint(status_code) +
          ": " + status_message
          );
      }

    // Read the response headers, which are terminated by a blank line.
    boost::asio::read_until(socket, response, "\r\n\r\n");

    // Process the response headers.
    std::string header;
    while (std::getline(response_stream, header) && header != "\r")
      {/* consume header lines */}

    // Write whatever content we already have to output.
    if (response.size() > 0)
      response_ << &response;

    // Read until EOF, writing data to output as we go.
    boost::system::error_code error;
    while (boost::asio::read(socket,
                             response,
                             boost::asio::transfer_at_least(1), error))
      {
        response_ << &response;
        if (error && error != boost::asio::error::eof)
          throw boost::system::system_error(error);
      }
  }

}
