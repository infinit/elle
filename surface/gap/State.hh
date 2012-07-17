#ifndef  SURFACE_GAP_STATE_HH
# define SURFACE_GAP_STATE_HH

# include <string>
# include <map>

# include <elle/format/json/fwd.hh>

# include <plasma/meta/Client.hh>

# include "gap.h"

namespace surface
{
  namespace gap
  {

    struct FileInfos
    {
      std::string mount_point;
      std::string network_id;
      std::string absolute_path;
      std::string relative_path;
    };

    struct User
    {
      std::string _id;
      std::string email;
      std::string public_key;
    };

    typedef ::plasma::meta::NetworkResponse Network;

    class Exception
      : public std::runtime_error
    {
    public:
      gap_Status const code;

    public:
      Exception(gap_Status code, std::string const& msg);
    };

    class State
    {
    private:
      plasma::meta::Client*             _api;

    public:
      State();
      ~State();

    ///
    /// Login & register
    ///
    private:
      std::map<std::string, User*> _users;
    public:
      void login(std::string const& email,
                 std::string const& password);
      void logout();
      std::string hash_password(std::string const& email,
                                std::string const& password);
      void register_(std::string const& fullname,
                     std::string const& email,
                     std::string const& password,
                     std::string const& activation_code);
      User const& user(std::string const& id);

    ///
    /// Launch and stop infinit instances
    ///
    public:
      void stop_watchdog();
      void launch_watchdog();

    ///
    /// Manage local device.
    ///
    public:
      bool has_device() const;
      void update_device(std::string const& name,
                         bool force_create = false);
    ///
    /// File infos
    ///
    private:
      std::map<std::string, FileInfos*> _files_infos;
    public:
      FileInfos const& file_infos(std::string const& abspath);
      void set_permissions(std::string const& user_id,
                           std::string const& abspath,
                           int permissions);

    ///
    /// Manipulate networks
    ///
    private:
      std::map<std::string, Network*>   _networks;
      bool                              _networks_dirty;
    public:
      std::map<std::string, Network*> const& networks();
      void create_network(std::string const& name);
      void refresh_networks();
      Network const& network(std::string const& id);

    private:
      std::string _watchdog_id() const;
      void _send_watchdog_cmd(std::string const& cmd,
                              elle::format::json::Dictionary const* = nullptr,
                              elle::format::json::Dictionary* = nullptr);
      void _reload_networks();
    };

  }
}


#endif

