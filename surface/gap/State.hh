#ifndef  SURFACE_GAP_STATE_HH
# define SURFACE_GAP_STATE_HH

# include <string>
# include <map>

# include <boost/filesystem.hpp>
# include <elle/format/json/fwd.hh>

# include <plasma/meta/Client.hh>
# include <plasma/trophonius/Client.hh>

# include "gap.h"

namespace surface
{
  namespace gap
  {
    struct FileInfos
    {
      std::string                 mount_point;
      std::string                 network_id;
      std::string                 absolute_path;
      std::string                 relative_path;
      std::map<std::string, int>  accesses;
    };

    struct User
    {
      std::string _id;
      std::string fullname;
      std::string email;
      std::string public_key;
    };

    typedef ::plasma::meta::NetworkResponse Network;

    struct NetworkStatus
    {
      std::string network_id;
      std::string mount_point;
    };

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
      plasma::meta::Client* _meta;
      plasma::trophonius::Client* _trophonius;

    public:
      State();
      ~State();

    ///
    /// Login & register
    ///
    private:
      std::map<std::string, User*> _users;
    public:
      /// Login to meta.
      void
      login(std::string const& email,
            std::string const& password);

      /// Logout from meta.
      void
      logout();

      /// Hash the password for the login and register methods.
      std::string
      hash_password(std::string const& email,
                    std::string const& password);

      /// Register a new user. The login is automatically done in case of
      /// success.
      void
      register_(std::string const& fullname,
                std::string const& email,
                std::string const& password,
                std::string const& activation_code);

      /// Retrieve a user by id or with its email.
      User const&
      user(std::string const& id);

      /// Retrieve a user by its public key.
      User const&
      user_from_public_key(std::string const& public_key);

      // Search users
      std::map<std::string, User const*>
      search_users(std::string const& text);

      /// Connect to trophonius
      void
      connect();

      /// Send message to user @id via trophonius
      void
      send_message(std::string const& recipient_id,
                   std::string const& message);

      void
      ask_notif(int i);

      std::string
      invite_user(std::string const& email);

      void
      send_file_to_new_user(std::string const& recipient_email,
                            std::string const& file_path);
      void
      send_file(std::string const& recipient_id,
                std::string const& file_path);

    private:
      User _me;

    ///
    /// Launch and stop infinit instances.
    ///
    public:
      /// Stop the watchdog process.
      void
      stop_watchdog();

      /// Start the watchdog process.
      void
      launch_watchdog();

    ///
    /// Manage local device.
    ///
    public:
      /// Check if the local device has been created.
      bool
      has_device() const;

      /// Create or update the local device.
      void
      update_device(std::string const& name,
                    bool force_create = false);
    ///
    /// File infos
    ///
    private:
      std::map<std::string, FileInfos*> _files_infos;
    public:
      /// Retrieve files infos.
      FileInfos const&
      file_infos(std::string const& abspath);

      /// Get size of a given path.
      size_t
      get_size(boost::filesystem::path const& path);

      std::string
      get_name(boost::filesystem::path const& path);

      /// Set the permissions for a file.
      void
      set_permissions(std::string const& user_id,
                      std::string const& abspath,
                      int permissions,
                      bool recursive = false);

    ///
    /// Manipulate networks
    ///
    private:
      std::map<std::string, Network*>       _networks;
      bool                                  _networks_dirty;
      std::map<std::string, NetworkStatus*> _networks_status;
      bool                                  _networks_status_dirty;

    public:
      /// Retrieve all networks.
      std::map<std::string, Network*> const& networks();

      /// Create a new network.
      std::string
      create_network(std::string const& name);

      /// Force the watchdog to check for new networks.
      void
      refresh_networks();

      /// Retrieve a network.
      Network const&
      network(std::string const& id);

      /// Retrieve all networks status.
      std::map<std::string, NetworkStatus*> const&
      networks_status();

      /// Retrieve a network status (from the watchdog).
      NetworkStatus const&
      network_status(std::string const& id);

      /// Add a user to a network with its mail or id.
      void
      network_add_user(std::string const& network_id,
                       std::string const& user);

      /// Accept file transfer.
      void
      accept_file_transfer(std::string const& transaction_id) {(void) transaction_id; }

      /// Deny file transfer.
      void
      deny_file_transfer(std::string const& transaction_id) {(void) transaction_id; }

      /// Trophonius binding.
    private:
      typedef std::map<int, plasma::trophonius::BasicHandler*> HandlerMap;
      HandlerMap _notification_handler;

    public:
      template<typename T>
      void
      attach_callback(std::function<void(T const*)> callback);

      bool
      poll();

     private:
      // Retrieve the current watchdog id.
      std::string
      _watchdog_id() const;

      // Send a command to the watchdog. If request is not null, it will be
      // sent along with the command, and if response is not null, it will be
      // filled with the command results.
      void
      _send_watchdog_cmd(std::string const& cmd,
                         elle::format::json::Dictionary const* request = nullptr,
                         elle::format::json::Dictionary* response = nullptr);

      void
      _reload_networks();
    };

  }
}


#endif
