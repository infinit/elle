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

namespace plasma
{
  namespace meta
  {
    struct LoginResponse : plasma::Response
    {
      std::string  token;
      std::string  fullname;
      std::string  email;
      std::string  identity;
      std::string  _id;
    };

    struct LogoutResponse : plasma::Response
    {};

    struct RegisterResponse : plasma::Response
    {};

    struct AskNotificationResponse : plasma::Response
    {};

    struct UserResponse : plasma::Response
    {
      std::string _id;
      std::string fullname;
      std::string email;
      std::string public_key;
    };

    struct UsersResponse : plasma::Response
    {
      std::list<std::string> users;
    };

    struct SendMessageResponse : plasma::Response
    {};

    struct NetworksResponse : plasma::Response
    {
      std::list<std::string> networks;
    };

    struct NetworkResponse : plasma::Response
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

    struct CreateNetworkResponse : plasma::Response
    {
      std::string             created_network_id;
    };

    struct UpdateNetworkResponse : plasma::Response
    {
      std::string             updated_network_id;
    };

    struct NetworkNodesResponse : plasma::Response
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

    namespace json = elle::format::json;

    class Client
    {
    public:
      Client(std::string const& server,
             uint16_t port,
             bool check_errors = true);
      ~Client();

    public:
      LoginResponse
      login(std::string const& email,
            std::string const& password);

      LogoutResponse
      logout();

      RegisterResponse
      register_(std::string const& email,
                std::string const& fullname,
                std::string const& password,
                std::string const& activation_code);

      UserResponse
      user(std::string const& id);

      UserResponse
      user_from_public_key(std::string const& public_key);

      UsersResponse
      search_users(std::string const& key);

      CreateDeviceResponse
      create_device(std::string const& name);

      UpdateDeviceResponse
      update_device(std::string const& _id,
                    std::string const& name);

      SendMessageResponse
      send_message(std::string const& sender_id,
                   std::string const& recipient_id,
                   std::string const& message);

      // DEBUG
      AskNotificationResponse
      debug_ask_notif(json::Dictionary const& dic);

      NetworkResponse
      network(std::string const& _id);

      NetworksResponse
      networks();

      CreateNetworkResponse
      create_network(std::string const& name);

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

      NetworkConnectDeviceResponse
      network_connect_device(std::string const& network_id,
                             std::string const& device_id,
                             std::string const* local_ip,
                             uint16_t local_port,
                             std::string const* external_ip = nullptr,
                             uint16_t external_port = 0);



    public:
      void token(std::string const& tok);
      std::string const& token() const;
      std::string const& identity() const;
      void identity(std::string const& str);
      std::string const& email() const;
      void email(std::string const& str);

    private:
      elle::HttpClient _client;
      std::string _token;
      std::string _identity;
      std::string _email;
    };
  }
}

#endif
