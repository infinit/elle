
#include <boost/asio.hpp>

#include <elle/print.hh>
#include <elle/format/json/Parser.hh>

#include "API.hh"


#define CRLF "\r\n"

namespace surface
{
  namespace gap
  {
    API::API(std::string const& server, uint16_t port)
      : _server(server)
      , _port(port)
      , _token()
    {}

    API::ObjectPtr
    API::post(std::string const& url,
              elle::format::json::Object const& params)
    {
      namespace ip = boost::asio::ip;

      boost::asio::io_service io_service;
      ip::tcp::socket socket(io_service);

        {
          ip::tcp::resolver resolver(io_service);
          ip::tcp::resolver::query query(this->_server, elle::sprint(this->_port));
          ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
          boost::asio::connect(socket, endpoint_iterator);
        }

        {
          boost::asio::streambuf request;
          std::ostream request_stream(&request);
          std::string body = params.repr();
          request_stream << "POST " << url << " HTTP/1.0" CRLF
                         << "Host: " << this->_server << CRLF
                         << "Accept: */*" CRLF
                         << "Content-Length: " << elle::sprint(body.size()) << CRLF
                         << "Connection: close" CRLF CRLF
                         << body;

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
          throw std::runtime_error(
              elle::sprint("Response returned with status code ", status_code));

      // Read the response headers, which are terminated by a blank line.
      boost::asio::read_until(socket, response, "\r\n\r\n");

      // Process the response headers.
      std::string header;
      while (std::getline(response_stream, header) && header != "\r")
        std::cout << "*** " << header << "\n";
      std::cout << "===\n";

      std::stringstream res;

      // Write whatever content we already have to output.
      if (response.size() > 0)
        {
          res << &response;
        }

      // Read until EOF, writing data to output as we go.
      boost::system::error_code error;
      while (boost::asio::read(socket,
                               response,
                               boost::asio::transfer_at_least(1), error))
        {
          res << &response;
          if (error && error != boost::asio::error::eof)
            throw boost::system::system_error(error);
        }
      return elle::format::json::Parser<>().Parse(res);
    }

  }
}

