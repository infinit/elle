#include <boost/asio.hpp>

#include <elle/log.hh>
#include <elle/print.hh>
#include <elle/serialize/JSONArchive.hh>
#include <elle/format/json/Dictionary.hxx>
#include <elle/serialize/ListSerializer.hxx>

#include "Client.hh"

#include <elle/idiom/Close.hh>

#define CRLF "\r\n"

ELLE_LOG_COMPONENT("infinit.plasma.meta.Client");

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

SERIALIZE_RESPONSE(plasma::meta::LogoutResponse, ar, res)
{
  (void) ar;
  (void) res;
}

SERIALIZE_RESPONSE(plasma::meta::RegisterResponse, ar, res)
{
  (void) ar;
  (void) res;
}

SERIALIZE_RESPONSE(plasma::meta::UserResponse, ar, res)
{
  ar & named("_id", res._id);
  ar & named("fullname", res.fullname);
  ar & named("email", res.email);
  ar & named("public_key", res.public_key);
}

SERIALIZE_RESPONSE(plasma::meta::UsersResponse, ar, res)
{
  ar & named("users", res.users);
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

SERIALIZE_RESPONSE(plasma::meta::NetworkNodesResponse, ar, res)
{
  ar & named("network_id", res.network_id);
  ar & named("nodes", res.nodes);
}

SERIALIZE_RESPONSE(plasma::meta::CreateNetworkResponse, ar, res)
{
  ar & named("created_network_id", res.created_network_id);
}
SERIALIZE_RESPONSE(plasma::meta::UpdateNetworkResponse, ar, res)
{
  ar & named("updated_network_id", res.updated_network_id);
}

SERIALIZE_RESPONSE(plasma::meta::NetworkResponse, ar, res)
{
  ar & named("_id", res._id);
  ar & named("name", res.name);
  ar & named("owner", res.owner);
  ar & named("model", res.model);
  try
    {
      ar & named("root_block", res.root_block);
      ar & named("root_address", res.root_address);
      ar & named("access_block", res.access_block);
      ar & named("access_address", res.access_address);
      ar & named("group_block", res.group_block);
      ar & named("group_address", res.group_address);
      ar & named("descriptor", res.descriptor);
    }
  catch (std::bad_cast const&)
    {
      if (Archive::mode != ArchiveMode::Input)
          throw;
      res.root_block = "";
      res.root_address = "";
      res.access_block = "";
      res.access_address = "";
      res.group_block = "";
      res.group_address = "";
      res.descriptor = "";
    }
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
      boost::asio::io_service   io_service;
      std::string               server;
      uint16_t                     port;
      std::string               token;
      std::string               identity;
      std::string               email;
      bool                      check_errors;

      Impl(std::string const& server,
           uint16_t port,
           bool check_errors)
        : io_service{}
        , server{server}
        , port{port}
        , token{}
        , identity{}
        , email{}
        , check_errors{check_errors}
      {}
    };

    // - Ctor & dtor ----------------------------------------------------------

    Client::Client(std::string const& server,
                   uint16_t port,
                   bool check_errors)
      : _impl{new Impl{server, port, check_errors}}
    {}

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
      return this->_get<UserResponse>("/user/" + id + "/view");
    }

    UserResponse
    Client::user_from_public_key(std::string const& public_key)
    {
      if (public_key.size() == 0)
        throw std::runtime_error("empty public key!");
      json::Dictionary request;
      request["public_key"] = public_key;
      return this->_post<UserResponse>("/user/from_public_key", request);
    }

    UsersResponse
    Client::search_users(std::string const& text)
    {
      json::Dictionary request;
      request["text"] = text;
      return this->_post<UsersResponse>("/user/search", request);
    }

    //- Devices ---------------------------------------------------------------

    CreateDeviceResponse
    Client::create_device(std::string const& name)
    {
      json::Dictionary request{std::map<std::string, std::string>{
          {"name", name},
      }};
      return this->_post<CreateDeviceResponse>("/device/create", request);
    }

    UpdateDeviceResponse
    Client::update_device(std::string const& _id,
                          std::string const& name)
    {
      json::Dictionary request{std::map<std::string, std::string>{
            {"_id", _id},
            {"name", name},
      }};

      return this->_post<UpdateDeviceResponse>("/device/update", request);

    }

    //- Networks --------------------------------------------------------------

    NetworksResponse
    Client::networks()
    {
      return this->_get<NetworksResponse>("/networks");
    }

    NetworkResponse
    Client::network(std::string const& _id)
    {
      return this->_get<NetworkResponse>("/network/" + _id + "/view");
    }

    NetworkNodesResponse
    Client::network_nodes(std::string const& _id)
    {
      return this->_get<NetworkNodesResponse>("/network/" + _id + "/nodes");
    }

    CreateNetworkResponse
    Client::create_network(std::string const& name)
    {
      json::Dictionary request{std::map<std::string, std::string>{
          {"name", name},
      }};
      return this->_post<CreateNetworkResponse>("/network/create", request);
    }

    UpdateNetworkResponse
    Client::update_network(std::string const& _id,
                           std::string const* name,
                           std::string const* root_block,
                           std::string const* root_address,
                           std::string const* access_block,
                           std::string const* access_address,
                           std::string const* group_block,
                           std::string const* group_address)
    {
      json::Dictionary request{std::map<std::string, std::string>{
            {"_id", _id},
      }};
      if (name != nullptr)
        request["name"] = *name;

      assert(((root_block == nullptr && root_address == nullptr) ||
              (root_block != nullptr && root_address != nullptr)) &&
             "Give both root block and root address or none of them");
      if (root_block != nullptr)
        request["root_block"] = *root_block;
      if (root_address != nullptr)
        request["root_address"] = *root_address;

      assert(((access_block == nullptr && access_address == nullptr) ||
              (access_block != nullptr && access_address != nullptr)) &&
             "Give both access block and access address or none of them");
      if (access_block != nullptr)
        request["access_block"] = *access_block;
      if (access_address != nullptr)
        request["access_address"] = *access_address;

      assert(((group_block == nullptr && group_address == nullptr) ||
              (group_block != nullptr && group_address != nullptr)) &&
             "Give both group block and group address or none of them");
      if (group_block != nullptr)
        request["group_block"] = *group_block;
      if (group_address != nullptr)
        request["group_address"] = *group_address;

      assert((
        (root_block == nullptr &&
         access_block == nullptr &&
         group_block == nullptr) ||
        (root_block != nullptr &&
         access_block != nullptr &&
         group_block != nullptr)
      ) && "root, access and group block are tied together.");

      return this->_post<UpdateNetworkResponse>("/network/update", request);
    }

    NetworkAddUserResponse
    Client::network_add_user(std::string const& network_id,
                             std::string const& user_id)
    {
      json::Dictionary request{std::map<std::string, std::string>{
          {"_id", network_id},
          {"user_id", user_id},
      }};
      return this->_post<NetworkAddUserResponse>("/network/add_user", request);
    }
    NetworkAddDeviceResponse
    Client::network_add_device(std::string const& network_id,
                               std::string const& device_id)
    {
      json::Dictionary request{std::map<std::string, std::string>{
          {"_id", network_id},
          {"device_id", device_id},
      }};
      return this->_post<NetworkAddDeviceResponse>("/network/add_device", request);
    }

    NetworkConnectDeviceResponse
    Client::network_connect_device(std::string const& network_id,
                                   std::string const& device_id,
                                   std::string const* local_ip,
                                   uint16_t local_port,
                                   std::string const* external_ip,
                                   uint16_t external_port)
    {
      json::Dictionary request{std::map<std::string, std::string>{
          {"_id", network_id},
          {"device_id", device_id},
      }};

      if (local_ip != nullptr)
      {
        json::Array local_addr;
        local_addr.push_back(*local_ip);
        local_addr.push_back(local_port);
        request["local"] = local_addr;
      }

      if (external_ip != nullptr)
      {
        json::Array external_addr;
        external_addr.push_back(*external_ip);
        external_addr.push_back(external_port);
        request["external"] = external_addr;
      }
      return this->_post<NetworkConnectDeviceResponse>(
          "/network/connect_device",
          request
      );
    }

    //- Properties ------------------------------------------------------------

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

    void
    Client::identity(std::string const& str)
    {
      _impl->identity = str;
    }

    std::string const&
    Client::email() const
    {
      return _impl->email;
    }
    void
    Client::email(std::string const& str)
    {
      _impl->email = str;
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
        {
          ELLE_TRACE("POST %s %s threw an error", url, req.repr());
          throw Exception(Error::network_error, err.what());
        }

      T ret;

      ELLE_TRACE("POST %s %s => %s", url, req.repr(), res.str());
      // deserialize response
      try
        { elle::serialize::InputJSONArchive(res, ret); }
      catch (std::exception const& err)
        { throw Exception(Error::invalid_content, err.what()); }

      if (!ret.success && _impl->check_errors)
        throw Exception(Error::server_error, ret.error);

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
        {
          ELLE_TRACE("GET %s threw an error", url);
          throw Exception(Error::network_error, err.what());
        }

      T ret;

      ELLE_TRACE("GET %s => %s", url, res.str());

      // deserialize response
      try
        { elle::serialize::InputJSONArchive(res, ret); }
      catch (std::exception const& err)
        { throw Exception(Error::invalid_content, err.what()); }

      if (!ret.success && _impl->check_errors)
        throw Exception(Error::server_error, ret.error);

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
}
