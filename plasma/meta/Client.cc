#include <boost/asio.hpp>

#include <elle/print.hh>
#include <elle/serialize/JSONArchive.hh>

#include "Client.hh"

#define CRLF "\r\n"

// - API responses serializers ------------------------------------------------

namespace elle
{
  namespace serialize
  {
    namespace
    {
      template<typename T>
      struct ResponseSerializer
      {
        template<typename Archive, typename Value>
        static void serialize(Archive&, Value&);
      };
    }
  }
}

#define SERIALIZE_RESPONSE(type, archive, value)                              \
  ELLE_SERIALIZE_NO_VERSION(type);                                            \
  ELLE_SERIALIZE_SIMPLE(type, archive, value, version)                        \
  {                                                                           \
    assert(version == 0);                                                     \
    archive & named("success", value.success);                                \
    if (!value.success)                                                       \
      {                                                                       \
        archive & named("error", value.error);                                \
        return;                                                               \
      }                                                                       \
    ResponseSerializer<type>::serialize(archive, value);                      \
  }                                                                           \
  template<> template<typename Archive, typename Value>                       \
  void elle::serialize::ResponseSerializer<type>::serialize(Archive& archive, \
                                                            Value& value)     \
  /**/

SERIALIZE_RESPONSE(plasma::meta::LoginResponse, ar, res)
{
  ar & named("token", res.token);
  ar & named("fullname", res.fullname);
  ar & named("email", res.fullname);
  ar & named("identity", res.identity);
}

SERIALIZE_RESPONSE(plasma::meta::RegisterResponse, ar, res) {}

namespace plasma
{
  namespace meta
  {

    // - Exception ------------------------------------------------------------

    Exception::Exception(Error error_code, std::string const& message)
      : std::runtime_error(message)
      , error_code(error_code)
    {}

    // - Client impl ----------------------------------------------------------

    struct Client::Impl
    {
      boost::asio::io_service io_service;
      std::string server;
      short port;
      std::string token;
    };

    // - Ctor & dtor ----------------------------------------------------------

    Client::Client(std::string const& server, short port)
      : _impl(new Impl)
    {
      _impl->server = server;
      _impl->port = port;
    }

    Client::~Client()
    {
      delete _impl;
      _impl = nullptr;
    }

    // - API calls ------------------------------------------------------------

    LoginResponse Client::login(std::string const& email,
                                std::string const& password)
    {
      json::Dictionary request{std::map<std::string, std::string>{
        {"email", email},
        {"password", password},
      }};
      auto res = this->_post<LoginResponse>("/user/login", request);
      if (res.success)
        _impl->token = res.token;
      return res;
    }

    RegisterResponse Client::register_(std::string const& email,
                                       std::string const& fullname,
                                       std::string const& password)
    {
      json::Dictionary request{std::map<std::string, std::string>{
        {"email", email},
        {"fullname", fullname},
        {"password", password},
      }};
      return this->_post<RegisterResponse>("/user/register", request);
    }

    // - Generic http POST and GET --------------------------------------------

    // XXX Should be rewritten in a cleaner way (using our tools).

    template<typename T>
    T Client::_post(std::string const& url, json::Object const& req)
    {
      std::string uri = _impl->server + url;

      namespace ip = boost::asio::ip;

      ip::tcp::socket socket(_impl->io_service);

        {
          ip::tcp::resolver resolver(_impl->io_service);
          ip::tcp::resolver::query query(_impl->server, elle::sprint(_impl->port));
          ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
          boost::asio::connect(socket, endpoint_iterator);
        }

        {
          boost::asio::streambuf request;
          std::ostream request_stream(&request);
          std::string body = req.repr();
          request_stream << "POST " << url << " HTTP/1.0" CRLF
                         << "Host: " << _impl->server << CRLF
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

      // deserialize from JSON stream
        {
          std::cerr << res.str() << std::endl;
          T ret;
          elle::serialize::InputJSONArchive(res, ret);
          return ret;
        }
    }

  }
}
