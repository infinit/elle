#include <elle/log.hh>
#include <elle/print.hh>
#include <elle/serialize/JSONArchive.hh>
#include <elle/format/json/Dictionary.hxx>
#include <elle/serialize/ListSerializer.hxx>

#include "Client.hh"

#include <elle/idiom/Close.hh>

ELLE_LOG_COMPONENT("infinit.plasma.meta.Client");

// - API responses serializers ------------------------------------------------
#define SERIALIZE_RESPONSE(type, archive, value)                              \
  ELLE_SERIALIZE_NO_FORMAT(type);                                             \
  ELLE_SERIALIZE_SIMPLE(type, archive, value, version)                        \
  {                                                                           \
    assert(version == 0);                                                     \
    archive & named("success", value._success);                              \
   if (!value.success())                                                      \
      {                                                                       \
        archive & named("error_code", value.response_code);                      \
        archive & named("error_details", value.response_details);                \
        return;                                                               \
      }                                                                       \
    ResponseSerializer<type>::serialize(archive, value);                      \
  }                                                                           \
  template<> template<typename Archive, typename Value>                       \
  void elle::serialize::ResponseSerializer<type>::serialize(Archive& archive, \
                                                            Value& value)     \


SERIALIZE_RESPONSE(plasma::meta::LoginResponse, ar, res)
{
  ar & named("token", res.token);
  ar & named("fullname", res.fullname);
  ar & named("email", res.fullname);
  ar & named("identity", res.identity);
  ar & named("_id", res._id);
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

SERIALIZE_RESPONSE(plasma::meta::SendMessageResponse, ar, res)
{
  (void) ar;
  (void) res;
}

SERIALIZE_RESPONSE(plasma::meta::AskNotificationResponse, ar, res)
{
  (void) ar;
  (void) res;
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
    namespace json = elle::format::json;

     // - Ctor & dtor ----------------------------------------------------------
    Client::Client(std::string const& server,
                   uint16_t port,
                   bool check_errors)
      : _client{server, port, check_errors}
      , _token{}
      , _identity{}
      , _email{}
    {

    }

    Client::~Client()
    {
    }

    // - API calls ------------------------------------------------------------
    LoginResponse Client::login(std::string const& email,
                                std::string const& password)
    {
      json::Dictionary request{std::map<std::string, std::string>{
        {"email", email},
        {"password", password},
      }};
      auto res = this->_client.post<LoginResponse>("/user/login", request);
      if (res.success())
        {
          this->token(res.token);
          _identity = res.identity;
          _email = email;
        }
      return res;
    }

    LogoutResponse
    Client::logout()
    {
      auto res = this->_client.get<LogoutResponse>("/user/logout");
      if (res.success())
        {
          _token = "";
          _identity = "";
          _email = "";
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
      return this->_client.post<RegisterResponse>("/user/register", request);
    }

    UserResponse
    Client::user(std::string const& id)
    {
      if (id.size() == 0)
        throw std::runtime_error("Wrong id");
      return this->_client.get<UserResponse>("/user/" + id + "/view");
    }

    UserResponse
    Client::user_from_public_key(std::string const& public_key)
    {
      if (public_key.size() == 0)
        throw std::runtime_error("empty public key!");
      json::Dictionary request;
      request["public_key"] = public_key;
      return this->_client.post<UserResponse>("/user/from_public_key", request);
    }

    UsersResponse
    Client::search_users(std::string const& text)
    {
      json::Dictionary request;
      request["text"] = text;
      return this->_client.post<UsersResponse>("/user/search", request);
    }

    //- Devices ---------------------------------------------------------------

    CreateDeviceResponse
    Client::create_device(std::string const& name)
    {
      json::Dictionary request{std::map<std::string, std::string>{
          {"name", name},
      }};
      return this->_client.post<CreateDeviceResponse>("/device/create", request);
    }

    UpdateDeviceResponse
    Client::update_device(std::string const& _id,
                          std::string const& name)
    {
      json::Dictionary request{std::map<std::string, std::string>{
            {"_id", _id},
            {"name", name},
      }};

      return this->_client.post<UpdateDeviceResponse>("/device/update", request);

    }

    SendMessageResponse
    Client::send_message(std::string const& sender_id,
                         std::string const& recipient_id,
                         std::string const& message)
    {
       json::Dictionary request{std::map<std::string, std::string>
         {
           {"recipient_id", recipient_id},
           {"sender_id", sender_id},
           {"message", message}}};

      auto res = this->_client.post<SendMessageResponse>("/user/connection", request);

      return res;
    }

    AskNotificationResponse
    Client::debug_ask_notif(json::Dictionary const& dic)
    {
      auto res = this->_client.post<AskNotificationResponse>("/debug", dic);

      return res;
    }

    //- Networks --------------------------------------------------------------

    NetworksResponse
    Client::networks()
    {
      return this->_client.get<NetworksResponse>("/networks");
    }

    NetworkResponse
    Client::network(std::string const& _id)
    {
      return this->_client.get<NetworkResponse>("/network/" + _id + "/view");
    }

    NetworkNodesResponse
    Client::network_nodes(std::string const& _id)
    {
      return this->_client.get<NetworkNodesResponse>("/network/" + _id + "/nodes");
    }

    CreateNetworkResponse
    Client::create_network(std::string const& name)
    {
      json::Dictionary request{std::map<std::string, std::string>{
          {"name", name},
      }};
      return this->_client.post<CreateNetworkResponse>("/network/create", request);
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

      return this->_client.post<UpdateNetworkResponse>("/network/update", request);
    }

    NetworkAddUserResponse
    Client::network_add_user(std::string const& network_id,
                             std::string const& user_id)
    {
      json::Dictionary request{std::map<std::string, std::string>{
          {"_id", network_id},
          {"user_id", user_id},
      }};
      return this->_client.post<NetworkAddUserResponse>("/network/add_user", request);
    }
    NetworkAddDeviceResponse
    Client::network_add_device(std::string const& network_id,
                               std::string const& device_id)
    {
      json::Dictionary request{std::map<std::string, std::string>{
          {"_id", network_id},
          {"device_id", device_id},
      }};
      return this->_client.post<NetworkAddDeviceResponse>("/network/add_device", request);
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
      return this->_client.post<NetworkConnectDeviceResponse>(
          "/network/connect_device",
          request
      );
    }

    //- Properties ------------------------------------------------------------

    void
    Client::token(std::string const& tok)
    {
      _token = tok;
      _client.token(tok);
    }

    std::string const&
    Client::token() const
    {
      return _client.token();
    }

    std::string const&
    Client::identity() const
    {
      return _identity;
    }

    void
    Client::identity(std::string const& str)
    {
      _identity = str;
    }

    std::string const&
    Client::email() const
    {
      return _email;
    }
    void
    Client::email(std::string const& str)
    {
      _email = str;
    }
  }
}
