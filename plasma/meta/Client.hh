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

# include <elle/HttpClient.hh>


namespace plasma
{
  namespace meta
  {
    namespace json = elle::format::json;

    using string = std::string;

    template <typename T>
    using list = std::list<T>;

    template <typename T, typename U>
    using pair = std::pair<T, U>;

    template <typename T, typename U>
    using map = std::map<T, U>;

    enum class Error: int
    {
# define ERR_CODE(name, value, comment)                                         \
      name = value,
# include <oracle/disciples/meta/error_code.hh.inc>
# undef ERR_CODE
    };

    /// Base class for every response
    struct Response
    {
      bool _success;
      int response_code; // should be a plasma::meta::Error
      string response_details;

      bool success() const
      {
        return _success;
      }
    };

    /////////////////////////
    struct DebugResponse : Response
    {
    };

    struct LoginResponse : Response
    {
      string  token;
      string  fullname;
      string  email;
      string  identity;
      string  _id;
    };

    struct LogoutResponse : Response
    {};

    struct RegisterResponse : Response
    {};

    struct PullNotificationResponse : Response
    {
      list<json::Dictionary> notifs;
      list<json::Dictionary> old_notifs;
    };

    struct ReadNotificationResponse : Response
    {};

    struct MessageResponse : Response
    {};

    struct User
    {
      string _id;
      string fullname;
      string email;
      string public_key;
      int    status;
    };

    struct UserResponse : User, Response
    {};

    // struct SwaggerResponse : UserResponse
    // {};

    struct SelfResponse : UserResponse
    {
      string identity;
    };

    struct InviteUserResponse : Response
    {
      string _id;
    };

    struct UsersResponse : Response
    {
      list<string> users;
    };

    struct SwaggersResponse : Response
    {
      list<string> swaggers;
    };

    struct TransactionsResponse : Response
    {
      list<string> transactions;
    };

    struct TransactionResponse:
      public Response,
      public ::plasma::Transaction
    {};


    struct CreateTransactionResponse : Response
    {
      string created_transaction_id;
    };

    struct UpdateTransactionResponse : Response
    {
      string updated_transaction_id;
    };

    struct NetworksResponse : Response
    {
      list<string> networks;
    };

    struct NetworkResponse : Response
    {
      string              _id;
      string              owner;
      string              name;
      string              model;
      string              root_block;
      string              root_address;
      string              group_block;
      string              group_address;
      string              descriptor;
      list<string>        users;
    };

    struct CreateNetworkResponse : Response
    {
      string             created_network_id;
    };

    struct DeleteNetworkResponse : Response
    {
      string             deleted_network_id;
    };

    struct UpdateNetworkResponse : Response
    {
      string             updated_network_id;
    };

    struct NetworkNodesResponse : Response
    {
      string             network_id;
      list<string>       nodes;
    };

    struct EndpointNodeResponse : Response
    {
      list<string>      externals;
      list<string>      locals;
    };

    typedef UpdateNetworkResponse NetworkAddUserResponse;
    typedef UpdateNetworkResponse NetworkAddDeviceResponse;
    typedef UpdateNetworkResponse NetworkConnectDeviceResponse;

    struct CreateDeviceResponse : Response
    {
      string             created_device_id;
      string             passport;
    };

    struct UpdateDeviceResponse : Response
    {
      string             updated_device_id;
      string             passport;
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


    typedef elle::Buffer UserIcon;

    class Client
    {
    private:
      elle::HTTPClient  _client;
      string            _identity;
      string            _email;

    public:
      Client(string const& server,
             uint16_t port,
             bool check_errors = true);
      ~Client();

    public:
      DebugResponse
      debug();

      LoginResponse
      login(string const& email,
            string const& password);

      LogoutResponse
      logout();

      RegisterResponse
      register_(string const& email,
                string const& fullname,
                string const& password,
                string const& activation_code,
                string const& picture_name = "",
                string const& picture_data = ""
      );

      UserResponse
      user(string const& id);

      UserIcon
      user_icon(string const& id);

      SelfResponse
      self();

      UserResponse
      user_from_public_key(string const& public_key);

      UsersResponse
      search_users(string const& key);

      SwaggersResponse
      get_swaggers();

      // SwaggerResponse
      // get_swagger(string const& id);

      CreateDeviceResponse
      create_device(string const& name);

      UpdateDeviceResponse
      update_device(string const& _id,
                    string const& name);

      InviteUserResponse
      invite_user(string const& email);

      TransactionResponse
      transaction(string const& _id);

      TransactionsResponse
      transactions();

      CreateTransactionResponse
      create_transaction(string const& recipient_id_or_email,
                         string const& first_filename,
                         size_t count,
                         size_t size,
                         bool is_dir,
                         string const& network_id,
                         string const& device_id);

      UpdateTransactionResponse
      update_transaction(string const& transaction_id,
                         plasma::TransactionStatus status,
                         string const& device_id = "",
                         string const& device_name = "");

      MessageResponse
      send_message(string const& recipient_id,
                   string const& sender_id, // DEBUG.
                   string const& message);

      PullNotificationResponse
      pull_notifications(int count, int offset = 0);

      ReadNotificationResponse
      notification_read();

      NetworkResponse
      network(string const& _id);

      NetworksResponse
      networks();

      CreateNetworkResponse
      create_network(string const& network_id);

      DeleteNetworkResponse
      delete_network(string const& network_id,
                     bool force = false);

      NetworkNodesResponse
      network_nodes(string const& network_id);

      UpdateNetworkResponse
      update_network(string const& _id,
                     string const* name,
                     string const* root_block,
                     string const* root_address,
                     string const* group_block,
                     string const* group_address);

      NetworkAddUserResponse
      network_add_user(string const& network_id,
                       string const& user_id);

      NetworkAddDeviceResponse
      network_add_device(string const& network_id,
                         string const& device_id);

      //
      // Frontend on _network_connect_device
      //
      NetworkConnectDeviceResponse
      network_connect_device(string const& network_id,
                             string const& device_id,
                             string const* local_ip,
                             uint16_t local_port,
                             string const* external_ip = nullptr,
                             uint16_t external_port = 0);

      //
      // Frontend on _network_connect_device
      //
      template <class Container1, class Container2>
      NetworkConnectDeviceResponse
      network_connect_device(string const& network_id,
                             string const& device_id,
                             Container1 const& local_endpoints,
                             Container2 const& public_endpoints);
      //
      // Frontend on _network_connect_device
      //
      template <class Container>
      NetworkConnectDeviceResponse
      network_connect_device(string const& network_id,
                             string const& device_id,
                             Container const& local_endpoints);

    private:

      using adapter_type = std::vector<pair<string, uint16_t>>;

      //
      // This member function is a adapter used to convert from any type of
      // container to `adapter_type´. This allow an interface handling all types
      // of iterable, but working with only one specific type.
      //
      NetworkConnectDeviceResponse
      _network_connect_device(string const& network_id,
                              string const& device_id,
                              adapter_type const& local_endpoints,
                              adapter_type const& public_endpoints);


    public:
      EndpointNodeResponse
      device_endpoints(string const& network_id, string const& device_id);


    public:
      void token(string const& tok);
      string const& token() const;
      string const& identity() const;
      void identity(string const& str);
      string const& email() const;
      void email(string const& str);
    };

    std::ostream&
    operator <<(std::ostream& out,
                Error e);
  }
}

#include "plasma/meta/Client.hxx"

#endif
