#ifndef  PLASMA_META_CLIENT_HH
# define PLASMA_META_CLIENT_HH

# include <functional>
# include <list>
# include <map>
# include <memory>
# include <stdexcept>

# include <elle/format/json/fwd.hh>
# include <elle/log.hh>

# include <plasma/plasma.hh>

# include <elle/HttpClient.hxx>
# include <surface/gap/gap.h>

namespace plasma
{
  namespace meta
  {
    namespace json = elle::format::json;

    /// Base class for every response
    struct Response
    {
      bool _success;
      int response_code;
      std::string response_details;

      bool success() const
      {
        return _success;//response_code < (int) elle::ResponseCode::error;
      }
    };

    /////////////////////////
    struct DebugResponse : Response
    {
    };

    struct LoginResponse : Response
    {
      std::string  token;
      std::string  fullname;
      std::string  email;
      std::string  identity;
      std::string  _id;
    };

    struct LogoutResponse : Response
    {};

    struct RegisterResponse : Response
    {};

    struct PullNotificationResponse : Response
    {
      std::list<json::Dictionary> notifs;
      std::list<json::Dictionary> old_notifs;
    };

    struct ReadNotificationResponse : Response
    {};

    struct MessageResponse : Response
    {};

    struct User
    {
      std::string _id;
      std::string fullname;
      std::string email;
      std::string public_key;
    };

    struct UserResponse : User, Response
    {};

    // struct SwaggerResponse : UserResponse
    // {};

    struct SelfResponse : UserResponse
    {
      std::string identity;
    };

    struct InviteUserResponse : Response
    {
      std::string _id;
    };

    struct UsersResponse : Response
    {
      std::list<std::string> users;
    };

    struct SwaggersResponse : Response
    {
      std::list<std::string> swaggers;
    };

    struct TransactionsResponse : Response
    {
      std::list<std::string> transactions;
    };

    struct TransactionResponse:
      public Response,
      public ::plasma::Transaction
    {};


    struct CreateTransactionResponse : Response
    {
      std::string created_transaction_id;
    };

    struct UpdateTransactionResponse : Response
    {
      std::string updated_transaction_id;
    };

    struct NetworksResponse : Response
    {
      std::list<std::string> networks;
    };

    struct NetworkResponse : Response
    {
      std::string              _id;
      std::string              owner;
      std::string              name;
      std::string              model;
      std::string              root_block;
      std::string              root_address;
      std::string              access_block;
      std::string              access_address;
      std::string              group_block;
      std::string              group_address;
      std::string              descriptor;
      std::list<std::string>   users;
    };

    struct CreateNetworkResponse : Response
    {
      std::string             created_network_id;
    };

    struct DeleteNetworkResponse : Response
    {
      std::string             deleted_network_id;
    };

    struct UpdateNetworkResponse : Response
    {
      std::string             updated_network_id;
    };

    struct NetworkNodesResponse : Response
    {
      std::string             network_id;
      std::list<std::string>  nodes;
    };

    typedef UpdateNetworkResponse NetworkAddUserResponse;
    typedef UpdateNetworkResponse NetworkAddDeviceResponse;
    typedef UpdateNetworkResponse NetworkConnectDeviceResponse;

    struct CreateDeviceResponse : Response
    {
      std::string             created_device_id;
      std::string             passport;
    };

    struct UpdateDeviceResponse : Response
    {
      std::string             updated_device_id;
      std::string             passport;
    };

    /// Callbacks for API calls.
    typedef std::function<void(LoginResponse const&)> LoginCallback;
    typedef std::function<void(RegisterResponse const&)> RegisterCallback;
    typedef std::function<void(NetworksResponse const&)> NetworksCallback;
    typedef std::function<void(NetworkResponse const&)> NetworkCallback;
    typedef std::function<void(CreateDeviceResponse const&)> CreateDeviceCallback;
    typedef std::function<void(UpdateDeviceResponse const&)> UpdateDeviceCallback;
    typedef std::function<void(UpdateNetworkResponse const&)> UpdateNetworkCallback;
    typedef std::function<void(NetworkNodesResponse const&)> NetworkNodesCallback;

    class Client
    {
    private:
      elle::HttpClient  _client;
      std::string       _token;
      std::string       _identity;
      std::string       _email;

    public:
      Client(std::string const& server,
             uint16_t port,
             bool check_errors = true);
      ~Client();

    public:
      DebugResponse
      debug();

      LoginResponse
      login(std::string const& email,
            std::string const& password);

      LogoutResponse
      logout();

      RegisterResponse
      register_(std::string const& email,
                std::string const& fullname,
                std::string const& password,
                std::string const& activation_code,
                std::string const& picture_name = "",
                std::string const& picture_data = ""
      );

      UserResponse
      user(std::string const& id);

      SelfResponse
      self();

      UserResponse
      user_from_public_key(std::string const& public_key);

      UsersResponse
      search_users(std::string const& key);

      SwaggersResponse
      get_swaggers();

      // SwaggerResponse
      // get_swagger(std::string const& id);

      CreateDeviceResponse
      create_device(std::string const& name);

      UpdateDeviceResponse
      update_device(std::string const& _id,
                    std::string const& name);

      InviteUserResponse
      invite_user(std::string const& email);

      TransactionResponse
      transaction(std::string const& _id);

      TransactionsResponse
      transactions();

      CreateTransactionResponse
      create_transaction(std::string const& recipient_id_or_email,
                         std::string const& first_filename,
                         size_t count,
                         size_t size,
                         bool is_dir,
                         std::string const& network_id,
                         std::string const& device_id);

      UpdateTransactionResponse
      update_transaction(std::string const& transaction_id,
                         int status,
                         std::string const& device_id = "",
                         std::string const& device_name = "");

      MessageResponse
      send_message(std::string const& recipient_id,
                   std::string const& sender_id, // DEBUG.
                   std::string const& message);

      PullNotificationResponse
      pull_notifications(int count, int offset = 0);

      ReadNotificationResponse
      notification_read();

      NetworkResponse
      network(std::string const& _id);

      NetworksResponse
      networks();

      CreateNetworkResponse
      create_network(std::string const& network_id);

      DeleteNetworkResponse
      delete_network(std::string const& network_id);

      NetworkNodesResponse
      network_nodes(std::string const& network_id);

      UpdateNetworkResponse
      update_network(std::string const& _id,
                     std::string const* name,
                     std::string const* root_block,
                     std::string const* root_address,
                     std::string const* access_block,
                     std::string const* access_address,
                     std::string const* group_block,
                     std::string const* group_address);

      NetworkAddUserResponse
      network_add_user(std::string const& network_id,
                       std::string const& user_id);

      NetworkAddDeviceResponse
      network_add_device(std::string const& network_id,
                         std::string const& device_id);

      //
      // Frontend on _network_connect_device
      //
      NetworkConnectDeviceResponse
      network_connect_device(std::string const& network_id,
                             std::string const& device_id,
                             std::string const* local_ip,
                             uint16_t local_port,
                             std::string const* external_ip = nullptr,
                             uint16_t external_port = 0);

      //
      // Frontend on _network_connect_device
      //
      template <class Container1, class Container2>
      NetworkConnectDeviceResponse
      network_connect_device(std::string const& network_id,
                             std::string const& device_id,
                             Container1 const& local_endpoints,
                             Container2 const& public_endpoints);
      //
      // Frontend on _network_connect_device
      //
      template <class Container>
      NetworkConnectDeviceResponse
      network_connect_device(std::string const& network_id,
                             std::string const& device_id,
                             Container const& local_endpoints);

    private:

      using adapter_type = std::vector<std::pair<std::string, uint16_t>>;

      //
      // This member function is a adapter used to convert from any type of
      // container to `adapter_type´. This allow an interface handling all types
      // of iterable, but working with only one specific type.
      //
      NetworkConnectDeviceResponse
      _network_connect_device(std::string const& network_id,
                              std::string const& device_id,
                              adapter_type const& local_endpoints,
                              adapter_type const& public_endpoints);



    public:
      void token(std::string const& tok);
      std::string const& token() const;
      std::string const& identity() const;
      void identity(std::string const& str);
      std::string const& email() const;
      void email(std::string const& str);
    };
  }
}

#include "plasma/meta/Client.hxx"

#endif
