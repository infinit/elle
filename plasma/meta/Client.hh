#ifndef  PLASMA_META_CLIENT_HH
# define PLASMA_META_CLIENT_HH

# include <functional>
# include <list>
# include <map>
# include <memory>
# include <stdexcept>

# include <elle/format/json/fwd.hh>
# include <elle/log.hh>

namespace plasma
{
  namespace meta
  {

    /// Base class for every response
    struct Response
    {
      bool success;
      std::string error;
    };

    struct LoginResponse : Response
    {
      std::string  token;
      std::string  fullname;
      std::string  email;
      std::string  identity;
    };

    struct LogoutResponse : Response
    {};

    struct RegisterResponse : Response
    {};

    struct UserResponse : Response
    {
      std::string _id;
      std::string fullname;
      std::string email;
      std::string public_key;
    };

    struct NetworksResponse : Response
    {
      std::list<std::string> networks;
    };

    struct NetworkResponse : Response
    {
      std::string              _id;
      std::string              name;
      std::string              model;
      std::string              root_block;
      std::string              root_address;
      std::string              descriptor;
      std::list<std::string>   devices;
      std::list<std::string>   users;
    };

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

    struct CreateNetworkResponse : Response
    {
      std::string             created_network_id;
    };

    struct UpdateNetworkResponse : Response
    {
      std::string             updated_network_id;
      std::string             descriptor;
      std::string             root_block;
      std::string             root_address;
    };

    struct NetworkNodesResponse : Response
    {
      std::string             network_id;
      std::list<std::string>  nodes;
    };

    struct NetworkAddUserResponse : Response
    {
      std::string             updated_network_id;
    };

    /// Error values.
    enum class Error
    {
      no_error = 0,
      network_error,
      invalid_content,
      server_error,
      callback_error,
      errback_error,
    };

    /// Exception thrown by Client methods
    class Exception
      : public std::runtime_error
    {
    public:
      Error const code;

    public:
      Exception(Error code, std::string const& message);
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
    private:
      struct Impl;
      Impl* _impl;

    public:
      Client(std::string const& server,
             short port,
             bool check_errors = true,
             elle::log::Logger& log = elle::log::default_logger);
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

      CreateDeviceResponse
      create_device(std::string const& name,
                    std::string const& local_address,
                    short port);

      UpdateDeviceResponse
      update_device(std::string const& _id,
                    std::string const* name,
                    std::string const* local_address,
                    short port);

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
                     std::list<std::string> const* users,
                     std::list<std::string> const* devices,
                     std::string const* root_block,
                     std::string const* root_address);


      NetworkAddUserResponse
      network_add_user(std::string const& network_id,
                       std::string const& user_id);

    public:
      void token(std::string const& tok);
      std::string const& token() const;
      std::string const& identity() const;
      std::string const& email() const;

    private:
      template<typename T>
      T _get(std::string const& url);
      template<typename T>
      T _post(std::string const& url, json::Object const& req);

      void _request(std::string const& url,
                    std::string const& method,
                    std::string const& body,
                    std::stringstream& response);
    };

  }
}


#endif

