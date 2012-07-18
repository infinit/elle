#include <boost/asio.hpp>

#include <elle/print.hh>
#include <elle/serialize/JSONArchive.hh>
#include <elle/format/json/Dictionary.hxx>
#include <elle/serialize/ListSerializer.hxx>

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

SERIALIZE_RESPONSE(plasma::meta::LogoutResponse, ar, res) {}
SERIALIZE_RESPONSE(plasma::meta::RegisterResponse, ar, res) {}

SERIALIZE_RESPONSE(plasma::meta::UserResponse, ar, res)
{
  ar & named("_id", res._id);
  ar & named("fullname", res.fullname);
  ar & named("email", res.email);
  ar & named("public_key", res.public_key);
}

SERIALIZE_RESPONSE(plasma::meta::CreateDeviceResponse, ar, res)
{
  ar & named("created_device_id", res.created_device_id);
  ar & named("passport", res.passport);
}

SERIALIZE_RESPONSE(plasma::meta::UpdateDeviceResponse, ar, res)
{
  ar & named("updated_device_id", res.updated_device_id);
  ar & named("passport", res.passport);
}

SERIALIZE_RESPONSE(plasma::meta::NetworksResponse, ar, res)
{
  ar & named("networks", res.networks);
}

SERIALIZE_RESPONSE(plasma::meta::CreateNetworkResponse, ar, res)
{
  ar & named("created_network_id", res.created_network_id);
}

SERIALIZE_RESPONSE(plasma::meta::NetworkResponse, ar, res)
{
  ar & named("_id", res._id);
  ar & named("name", res.name);
  ar & named("model", res.model);
  ar & named("root_block", res.root_block);
  ar & named("root_address", res.root_address);
  ar & named("descriptor", res.descriptor);
  ar & named("devices", res.devices);
  ar & named("users", res.users);
}

namespace plasma
{
  namespace meta
  {

    // - Exception ------------------------------------------------------------

    Exception::Exception(Error code, std::string const& message)
      : std::runtime_error(message)
      , code(code)
    {}

    // - Client impl ----------------------------------------------------------

    struct Client::Impl
    {
      boost::asio::io_service io_service;
      std::string server;
      short port;
      std::string token;
      std::string identity;
      std::string email;
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
        {
          _impl->token = res.token;
          _impl->identity = res.identity;
          _impl->email = email;
        }
      return res;
    }

    LogoutResponse
    Client::logout()
    {
      auto res = this->_get<LogoutResponse>("/user/logout");
      if (res.success)
        {
          _impl->token = "";
          _impl->identity = "";
          _impl->email = "";
        }
      return res;
    }

    RegisterResponse
    Client::register_(std::string const& email,
                      std::string const& fullname,
                      std::string const& password,
                      std::string const& activation_code)
    {
      json::Dictionary request{std::map<std::string, std::string>{
        {"email", email},
        {"fullname", fullname},
        {"password", password},
        {"activation_code", activation_code},
      }};
      return this->_post<RegisterResponse>("/user/register", request);
    }

    UserResponse
    Client::user(std::string const& id)
    {
      if (id.size() == 0)
        throw std::runtime_error("Wrong id");
      return this->_get<UserResponse>("/user/" + id);
    }

    CreateDeviceResponse
    Client::create_device(std::string const& name,
                          std::string const& local_address,
                          short port)
    {
      json::Dictionary request{std::map<std::string, std::string>{
          {"name", name},
          {"local_ip", local_address},
          {"local_port", elle::sprint(port)},
      }};
      return this->_post<CreateDeviceResponse>("/devices", request);
    }

    UpdateDeviceResponse
    Client::update_device(std::string const& _id,
                          std::string const* name,
                          std::string const* local_address,
                          short port)
    {
      json::Dictionary request{std::map<std::string, std::string>{
            {"_id", _id},
      }};

      if (name != nullptr)
        request["name"] = *name;
      if (local_address != nullptr)
        request["local_ip"] = *local_address;
      if (port != 0)
        request["local_port"] = port;

      return this->_post<UpdateDeviceResponse>("/devices", request);

    }

    NetworksResponse
    Client::networks()
    {
      return this->_get<NetworksResponse>("/networks");
    }

    NetworkResponse
    Client::network(std::string const& _id)
    {
      return this->_get<NetworkResponse>("/network/" + _id);
    }

    CreateNetworkResponse
    Client::create_network(std::string const& name)
    {
      json::Dictionary request{std::map<std::string, std::string>{
          {"name", name},
      }};
      return this->_post<CreateNetworkResponse>("/network", request);
    }

    void
    Client::token(std::string const& tok)
    {
      _impl->token = tok;
    }

    std::string const&
    Client::token() const
    {
      return _impl->token;
    }

    std::string const&
    Client::identity() const
    {
      return _impl->identity;
    }

    std::string const&
    Client::email() const
    {
      return _impl->email;
    }

    // - Generic http POST and GET --------------------------------------------

    // XXX Should be rewritten in a cleaner way (using our tools).

    template<typename T>
    T Client::_post(std::string const& url, json::Object const& req)
    {
      std::stringstream res;

      // http request
      try
        { this->_request(url, "POST", req.repr(), res); }
      catch (std::exception const& err)
        { throw Exception(Error::network_error, err.what()); }

      T ret;

      // deserialize response
      try
        { elle::serialize::InputJSONArchive(res, ret); }
      catch (std::exception const& err)
        { throw Exception(Error::invalid_content, err.what()); }

      return ret;
    }

    template<typename T>
    T Client::_get(std::string const& url)
    {
      std::stringstream res;

      // http request
      try
        { this->_request(url, "GET", "", res); }
      catch (std::exception const& err)
        { throw Exception(Error::network_error, err.what()); }

      T ret;

      // deserialize response
      try
        { elle::serialize::InputJSONArchive(res, ret); }
      catch (std::exception const& err)
        { throw Exception(Error::invalid_content, err.what()); }

      return ret;
    }

    void Client::_request(std::string const& url,
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
        if (_impl->token.size())
          request_stream << "Authorization: " << _impl->token << CRLF;

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
          throw std::runtime_error("Response returned with status code " +
              elle::sprint(status_code));
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
}
