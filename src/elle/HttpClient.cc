#include <boost/asio.hpp>

#include <elle/HttpClient.hh>

#include <elle/log.hh>
#include <elle/print.hh>
#include <elle/serialize/JSONArchive.hh>

# define CRLF "\r\n"

ELLE_LOG_COMPONENT("elle.HTTPClient");

namespace elle
{
  //- Exception ---------------------------------------------------------------
  HTTPException::HTTPException(ResponseCode code, std::string const& message)
    : std::runtime_error(message)
    , code(code)
  {}

  bool
  HTTPException::operator ==(HTTPException const& e)
  {
    return (this->code == e.code);
  }

  //- Request -----------------------------------------------------------------

  struct Request::Impl
  {
    HTTPClient&                                   client;
    std::string                                   method;
    std::string                                   url;
    std::string                                   content_type;
    std::unordered_map<std::string, std::string>  headers;
    std::unordered_map<std::string, std::string>  post_fields;
    std::stringstream                             response;
    // Reading body flushes it.
    mutable std::unique_ptr<std::istream>         body;

    Impl(HTTPClient& client,
         std::string const& method,
         std::string const& url)
      : client(client)
      , method{method}
      , url{url}
    {}
  };

  Request::Request(HTTPClient& client,
                   std::string const& method,
                   std::string const& url)
    : _this{new Impl{client, method, url}}
  {}

  Request::Request(Request&& other)
    : _this{std::move(other._this)}
  {}

  Request::~Request()
  {}

  Request&
  Request::body(std::unique_ptr<std::istream>&& input)
  {
    _this->body = std::move(input);

    return *this;
  }

  // properties
  std::unique_ptr<std::istream>
  Request::body() const
  {
    if (!_this->post_fields.empty())
    {
      ELLE_ASSERT(_this->body == nullptr);

      std::string body;
      bool first = true;
      for (auto const& pair: _this->post_fields)
      {
        if (!first)
          body += "&";
        else
          first = false;

        body += pair.first + "=" + pair.second; //XXX must be encoded.
      }

      return std::unique_ptr<std::istream>{new std::stringstream{body}};
    }

    ELLE_ASSERT(_this->body != nullptr);

    return std::move(_this->body);
  }

  bool
  Request::has_body() const
  {
    return (!_this->post_fields.empty() or _this->body != nullptr);
  }


  std::string
  Request::body_string() const
  {
    if (!this->has_body())
      return "";

    return static_cast<std::stringstream const&>(
      std::stringstream() << this->body()->rdbuf()
    ).str();
  }

  Request&
  Request::body_string(std::string const& str)
  {
    _this->body.reset(new std::stringstream{str});

    return *this;
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
  Request::user_agent(std::string const& str)
  {
    _this->headers.insert(std::make_pair("User-Agent", str));

    return *this;
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

  Request&
  Request::fire()
  {
    _this->client.fire(*this);

    return *this;
  }

  std::stringstream&
  Request::response()
  {
    return _this->response;
  }

  //- HTTPClient --------------------------------------------------------------
  struct HTTPClient::Impl
  {
    /*-----------.
    | Attributes |
    `-----------*/
    boost::asio::io_service   io_service;
    std::string               server;
    uint16_t                  port;
    bool                      check_errors;
    std::string               user_agent;

    /*-------------.
    | Construction |
    `-------------*/
    Impl(std::string const& server,
         uint16_t port,
         std::string const& user_agent)
      : io_service{}
      , server{server}
      , port{port}
      , check_errors{check_errors}
      , user_agent{user_agent}
    {}
  };

  HTTPClient::HTTPClient(std::string const& server,
                         uint16_t port,
                         std::string const& user_agent)
    : _impl{new Impl{server, port, user_agent}}
    , _token{}
  {}

  HTTPClient::~HTTPClient()
  {}

  elle::Buffer
  HTTPClient::get_buffer(std::string const& url)
  {
    // XXX not optimized (mulptiple copies).
    auto req = this->request("GET", url);
    req.fire();

    elle::Buffer buf;
    std::streamsize sz = 0;
    while (!req.response().eof())
    {
      buf.size(sz + 4096);
      sz += req.response().read((char*) buf.mutable_contents(), 4096).gcount();
    }
    buf.size(sz);
    return buf;
  }

  Request
  HTTPClient::request(std::string const& method,
                      std::string const& url)
  {
    return Request{*this, method, url};
  }

  void
  HTTPClient::fire(Request& request)
  {
    std::string uri = _impl->server + request.url();

    namespace ip = boost::asio::ip;

    ip::tcp::socket socket(_impl->io_service);

    ip::tcp::resolver resolver(_impl->io_service);
    ip::tcp::resolver::query query(_impl->server, elle::sprint(_impl->port));
    ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    boost::asio::connect(socket, endpoint_iterator);

    std::string body = request.body_string();
    {
      boost::asio::streambuf request_buf;
      std::ostream request_stream(&request_buf);

      request_stream << request.method() << ' ' << request.url() << " HTTP/1.0" CRLF
                     << "Host: " << _impl->server << CRLF
                     << (request.has_header("User-Agent")
                         ? std::string("")
                         : std::string("User-Agent: ") + this->_impl->user_agent + std::string(CRLF))
                     << "Connection: close" CRLF
                     << request.headers_string();
      if (_token.size())
      {
        request_stream << "Authorization: " << _token << CRLF;
      }
      if (body.size())
        request_stream << "Content-Length: " << elle::sprint(body.size()) << CRLF
                       << "Content-Type: " << request.content_type() << CRLF CRLF
                       << body;
      else
        request_stream << CRLF;

      ELLE_DEBUG("sending the following request to server '%s': '%s'",
                 uri, body);

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
  // void
  // HTTPClient::_request(std::string const& url,
  //                      std::string const& method,
  //                      std::string const& body,
  //                      std::stringstream& response_)
  // {
  //   //struct STACK {~STACK() { assert(false && "TOUT VA BIEN"); }} stack;
  //   std::string uri = _impl->server + url;

  //   namespace ip = boost::asio::ip;

  //   ip::tcp::socket socket(_impl->io_service);

  //   ip::tcp::resolver resolver(_impl->io_service);
  //   ip::tcp::resolver::query query(_impl->server, elle::sprint(_impl->port));
  //   ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
  //   //XXX which way is better ?
  //   //boost::asio::connect(socket, endpoint_iterator);
  //   socket.connect(*endpoint_iterator);

  //   {
  //     boost::asio::streambuf request;
  //     std::ostream request_stream(&request);
  //     request_stream << method << ' ' << url << " HTTP/1.0" CRLF
  //                    << "Host: " << this->_impl->server << CRLF
  //       // User agent management suxx.
  //                    << "User-Agent: MetaClient" CRLF
  //                    << "Connection: close" CRLF;
  //     if (_token.size())
  //       {
  //         request_stream << "Authorization: " << _token << CRLF;
  //       }
  //     if (body.size())
  //       request_stream << "Content-Length: " << elle::sprint(body.size()) << CRLF
  //                      << "Content-Type: application/json" << CRLF CRLF
  //                      << body;
  //     else
  //       request_stream << CRLF;

  //     // Send the request.
  //     boost::asio::write(socket, request);
  //   }

  //   // Read the response status line. The response streambuf will automatically
  //   // grow to accommodate the entire line. The growth may be limited by passing
  //   // a maximum size to the streambuf constructor.
  //   boost::asio::streambuf response;
  //   boost::asio::read_until(socket, response, "\r\n");

  //   // Check that response is OK.
  //   std::istream response_stream(&response);
  //   std::string http_version;
  //   response_stream >> http_version;
  //   unsigned int status_code;
  //   response_stream >> status_code;
  //   std::string status_message;
  //   std::getline(response_stream, status_message);
  //   if (!response_stream || http_version.substr(0, 5) != "HTTP/")
  //     throw std::runtime_error("Invalid response");
  //   if (status_code != 200)
  //     {
  //       //struct STACK {~STACK() { ::exit(12); }} stack;
  //       throw std::runtime_error(
  //         "Cannot " + method + " '" + body + "' " + uri +
  //         " returned HTTP code " + elle::sprint(status_code) +
  //         ": " + status_message
  //         );
  //     }

  //   // Read the response headers, which are terminated by a blank line.
  //   boost::asio::read_until(socket, response, "\r\n\r\n");

  //   // Process the response headers.
  //   std::string header;
  //   while (std::getline(response_stream, header) && header != "\r")
  //     {/* consume header lines */}

  //   // Write whatever content we already have to output.
  //   if (response.size() > 0)
  //     response_ << &response;

  //   // Read until EOF, writing data to output as we go.
  //   boost::system::error_code error;
  //   while (boost::asio::read(socket,
  //                            response,
  //                            boost::asio::transfer_at_least(1), error))
  //     {
  //       response_ << &response;
  //       if (error && error != boost::asio::error::eof)
  //         throw boost::system::system_error(error);
  //     }
  // }

  // std::stringstream
  // HTTPClient::post(std::string const& url, elle::format::json::Object const& req)
  // {
  //   ELLE_LOG_COMPONENT("elle.HTTPClient");

  //   std::stringstream res;

  //   // http request
  //   try
  //   {
  //     this->_request(url, "POST", req.repr(), res);
  //   }
  //   catch (std::exception const& err)
  //   {
  //     ELLE_TRACE("POST %s %s threw an error", url, req.repr());
  //     throw HTTPException(ResponseCode::internal_server_error, err.what());
  //   }

  //   return res;
  // }

  // std::stringstream
  // HTTPClient::get(std::string const& url)
  // {
  //   ELLE_LOG_COMPONENT("elle.HTTPClient");
  //   std::stringstream res;

  //   // http request
  //   try
  //   { this->_request(url, "GET", "", res); }
  //   catch (std::runtime_error const& err)
  //   {
  //     ELLE_TRACE("GET %s threw an error", url);
  //     throw HTTPException(ResponseCode::internal_server_error, err.what());
  //   }

  //   return res;
  // }

  bool
  HTTPClient::put(std::string const& url,
                  elle::format::json::Object const& data)
  {
    // http request
    try
    {
      auto req = this->request("PUT", url);
      req.body_string(data.repr()).fire();
    }
    catch (std::exception const& err)
    {
      ELLE_TRACE("PUT %s %s threw an error", url, data.repr());
      throw HTTPException(ResponseCode::internal_server_error, err.what());
    }

    return true;
  }
}
