#include "HttpClient.hxx"

#include <elle/log.hh>
#include <elle/print.hh>
#include <elle/serialize/JSONArchive.hh>

# define CRLF "\r\n"

namespace elle
{
// - Exception ------------------------------------------------------------
  HTTPException::HTTPException(ResponseCode code, std::string const& message)
    : std::runtime_error(message)
    , code(code)
  {}


// - HttpClient ----------------------------------------------------------
  HttpClient::HttpClient(std::string const& server,
                         uint16_t port,
                         bool check_errors)
    : _impl{new Impl{server, port, check_errors}}
    , _token{}
    {
    }

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
          elle::print("token:", _token);
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
