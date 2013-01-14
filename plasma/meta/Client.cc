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
    enforce(version == 0);                                                    \
    archive & named("success", value._success);                               \
   if (!value.success())                                                      \
      {                                                                       \
        archive & named("error_code", value.response_code);                   \
        archive & named("error_details", value.response_details);             \
        return;                                                               \
      }                                                                       \
    ResponseSerializer<type>::serialize(archive, value);                      \
  }                                                                           \
  template<> template<typename Archive, typename Value>                       \
  void elle::serialize::ResponseSerializer<type>::serialize(Archive& archive, \
                                                            Value& value)     \
/**/

namespace elle
{
  namespace serialize
  {
    template<typename T>
    struct ResponseSerializer
    {
      ELLE_SERIALIZE_BASE_CLASS_MIXIN_TN(T, 0)

      template<typename Archive, typename Value>
      static void serialize(Archive&, Value&);
    };
  }
}

SERIALIZE_RESPONSE(plasma::meta::DebugResponse, ar, res)
{
  (void) ar;
  (void) res;
};


SERIALIZE_RESPONSE(plasma::meta::LoginResponse, ar, res)
{
  ar & named("token", res.token);
  ar & named("fullname", res.fullname);
  ar & named("email", res.email);
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
  ar & named("status", res.status);
}

// SERIALIZE_RESPONSE(plasma::meta::SwaggerResponse, ar, res)
// {
//   ar & named("_id", res._id);
//   ar & named("fullname", res.fullname);
//   ar & named("email", res.email);
//   ar & named("public_key", res.public_key);
// }

SERIALIZE_RESPONSE(plasma::meta::SelfResponse, ar, res)
{
  ar & named("_id", res._id);
  ar & named("fullname", res.fullname);
  ar & named("email", res.email);
  ar & named("public_key", res.public_key);
  ar & named("identity", res.identity);
}

SERIALIZE_RESPONSE(plasma::meta::UsersResponse, ar, res)
{
  ar & named("users", res.users);
}

SERIALIZE_RESPONSE(plasma::meta::SwaggersResponse, ar, res)
{
  ar & named("swaggers", res.swaggers);
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

SERIALIZE_RESPONSE(plasma::meta::InviteUserResponse, ar, res)
{
  ar & named("_id", res._id);
}

SERIALIZE_RESPONSE(plasma::meta::TransactionResponse, ar, res)
{
  // XXX see plasma/plasma.hxx
  ar & named("transaction_id", res.transaction_id);
  ar & named("sender_id", res.sender_id);
  ar & named("sender_fullname", res.sender_fullname);
  ar & named("sender_device_id", res.sender_device_id);
  ar & named("recipient_id", res.recipient_id);
  ar & named("recipient_fullname", res.recipient_fullname);
  ar & named("recipient_device_id", res.recipient_device_id);
  ar & named("recipient_device_name", res.recipient_device_id);
  ar & named("network_id", res.network_id);
  ar & named("first_filename", res.first_filename);
  ar & named("files_count", res.files_count);
  ar & named("total_size", res.total_size);
  ar & named("is_directory", res.is_directory);
  ar & named("status", res.status);
  ar & named("message", res.message);
}

SERIALIZE_RESPONSE(plasma::meta::TransactionsResponse, ar, res)
{
  ar & named("transactions", res.transactions);
}

SERIALIZE_RESPONSE(plasma::meta::CreateTransactionResponse, ar, res)
{
  ar & named("created_transaction_id", res.created_transaction_id);
}

SERIALIZE_RESPONSE(plasma::meta::UpdateTransactionResponse, ar, res)
{
  ar & named("updated_transaction_id", res.updated_transaction_id);
}

SERIALIZE_RESPONSE(plasma::meta::MessageResponse, ar, res)
{
  (void) ar;
  (void) res;
}

SERIALIZE_RESPONSE(plasma::meta::PullNotificationResponse, ar, res)
{
  ar & named("notifs", res.notifs);
  ar & named("old_notifs", res.old_notifs);
}

SERIALIZE_RESPONSE(plasma::meta::ReadNotificationResponse, ar, res)
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

SERIALIZE_RESPONSE(plasma::meta::DeleteNetworkResponse, ar, res)
{
  ar & named("deleted_network_id", res.deleted_network_id);
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
      : _client{server, port, "MetaClient", check_errors}
      , _token{}
      , _identity{}
      , _email{}
    {

    }

    Client::~Client()
    {
    }

    // - API calls ------------------------------------------------------------
    // XXX add login with token method.
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
                      std::string const& activation_code,
                      std::string const& picture_name,
                      std::string const& picture_data)
    {
      json::Dictionary request{std::map<std::string, std::string>{
        {"email", email},
        {"fullname", fullname},
        {"password", password},
        {"activation_code", activation_code},
        {"picture_name", picture_name},
        {"picture_data", picture_data},
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

    UserIcon
    Client::user_icon(std::string const& id)
    {
      return this->_client.get_buffer("/user/" + id + "/icon");
    }

    SelfResponse
    Client::self()
    {
      return this->_client.get<SelfResponse>("/self");
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

    SwaggersResponse
    Client::get_swaggers()
    {
      return this->_client.get<SwaggersResponse>("/user/swaggers");
    }

    // SwaggerResponse
    // Client::get_swagger(std::string const& id)
    // {
    //   return this->_client.get<SwaggerResponse>("/user/" + id + "/view");
    // }


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

    InviteUserResponse
    Client::invite_user(std::string const& email)
    {
      json::Dictionary request{std::map<std::string, std::string>
        {
          {"email", email}
        }};

      auto res = this->_client.post<InviteUserResponse>("/user/invite", request);

      return res;
    }

    CreateTransactionResponse
    Client::create_transaction(std::string const& recipient_id_or_email,
                               std::string const& first_filename,
                               size_t count,
                               size_t size,
                               bool is_dir,
                               std::string const& network_id,
                               std::string const& device_id)
    {
      json::Dictionary request{std::map<std::string, std::string>{
          {"recipient_id_or_email", recipient_id_or_email},
          {"first_filename", first_filename},
          {"device_id", device_id},
          {"network_id", network_id},
      }};
      request["total_size"] = size;
      request["is_directory"] = is_dir;
      request["files_count"] = count;

      auto res = this->_client.post<CreateTransactionResponse>("/transaction/create", request);

      return res;
    }

    UpdateTransactionResponse
    Client::update_transaction(std::string const& transaction_id,
                               int status,
                               std::string const& device_id,
                               std::string const& device_name)
    {
      json::Dictionary request{std::map<std::string, std::string>
        {
          {"transaction_id", transaction_id},
        }};

      request["status"] = status;
      if (device_id.length() > 0)
        request["device_id"] = device_id;
      if (device_name.length() > 0)
        request["device_name"] = device_name;

      ELLE_DEBUG("Update '%s' transaction with device '%s'. New status '%ui'",
                 transaction_id,
                 device_name,
                 status);

      UpdateTransactionResponse res;

      switch(status)
      {
        case gap_TransactionStatus::gap_transaction_status_accepted:
          res = this->_client.post<UpdateTransactionResponse>("/transaction/accept", request);
          break;
        case gap_TransactionStatus::gap_transaction_status_started:
          res = this->_client.post<UpdateTransactionResponse>("/transaction/start", request);
          break;
        case gap_TransactionStatus::gap_transaction_status_canceled:
          res = this->_client.post<UpdateTransactionResponse>("/transaction/cancel", request);
          break;
        case gap_TransactionStatus::gap_transaction_status_finished:
          res = this->_client.post<UpdateTransactionResponse>("/transaction/finish", request);
          break;
        default:
          ELLE_WARN("You are not able to change transaction status to '%i'.",
            status);

      }

      return res;
    }

    TransactionResponse
    Client::transaction(std::string const& _id)
    {
      return this->_client.get<TransactionResponse>("/transaction/" + _id + "/view");
    }

    TransactionsResponse
    Client::transactions()
    {
      return this->_client.get<TransactionsResponse>("/transactions");
    }

    MessageResponse
    Client::send_message(std::string const& recipient_id,
                         std::string const& sender_id,
                         std::string const& message)
    {
      json::Dictionary request{std::map<std::string, std::string>
        {
          {"recipient_id", recipient_id},
          {"sender_id", sender_id},
          {"message", message},
        }};

      // FIXME: Time.h ????
      request["time"] = 0;
      request["notification_id"] = 217;

      // FIXME: /user/message
      auto res = this->_client.post<MessageResponse>("/debug", request);

      return res;
    }

    DebugResponse
    Client::debug()
    {
      return this->_client.get<DebugResponse>("/scratchit");
    }

    PullNotificationResponse
    Client::pull_notifications(int count, int offset)
    {
      json::Dictionary request{std::map<std::string, std::string>
      {
      }};

      request["count"] = count;
      request["offset"] = offset;

      auto res = this->_client.post<PullNotificationResponse>("/notification/get",
                                                              request);

      return res;
    }

    ReadNotificationResponse
    Client::notification_read()
    {
      return this->_client.get<ReadNotificationResponse>("/notification/read");
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
    Client::create_network(std::string const& network_id)
    {
      json::Dictionary request{std::map<std::string, std::string>{
          {"name", network_id},
      }};
      return this->_client.post<CreateNetworkResponse>("/network/create", request);
    }

    DeleteNetworkResponse
    Client::delete_network(std::string const& network_id,
                           bool force)
    {
      json::Dictionary request{std::map<std::string, std::string>{
          {"network_id", network_id},
      }};
      request["force"] = force;
      return this->_client.post<DeleteNetworkResponse>("/network/delete", request);
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
        adapter_type local_adapter;
        adapter_type public_adapter;

        local_adapter.emplace_back(*local_ip, local_port);
        public_adapter.emplace_back(*external_ip, external_port);

        return this->_network_connect_device(network_id,
                                             device_id,
                                             local_adapter,
                                             public_adapter);
    }

    NetworkConnectDeviceResponse
    Client::_network_connect_device(std::string const& network_id,
                                    std::string const& device_id,
                                    adapter_type const& local_endpoints,
                                    adapter_type const& public_endpoints)
      {
        json::Dictionary request{
          std::map<std::string, std::string>{
                {"_id", network_id},
                {"device_id", device_id},
          }
        };

        json::Array local_addrs;
        for (auto& a: local_endpoints)
          {
            json::Dictionary endpoint;

            endpoint["ip"] = a.first;
            endpoint["port"] = a.second;
            local_addrs.push_back(endpoint);
          }

        request["locals"] = local_addrs;

        json::Array public_addrs;
        for (auto& a : public_endpoints)
         {
           json::Dictionary pub_addr;

           pub_addr["ip"] = a.first;
           pub_addr["port"] = a.second;
           public_addrs.push_back(pub_addr);
         }

        request["externals"] = public_addrs;

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
