#ifndef  SURFACE_GAP_STATE_HH
# define SURFACE_GAP_STATE_HH

# include <functional>
# include <map>
# include <string>
# include <unordered_set>

# include <boost/filesystem.hpp>
# include <elle/format/json/fwd.hh>

# include <nucleus/neutron/Permissions.hh>

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

    // Used to represent all users in the state class.
    typedef ::plasma::meta::User User;
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

    using ::plasma::Transaction;
    using ::plasma::trophonius::Notification;
    using ::plasma::trophonius::TransactionNotification;
    using ::plasma::trophonius::TransactionStatusNotification;
    using ::plasma::trophonius::UserStatusNotification;
    using ::plasma::trophonius::MessageNotification;
    using ::plasma::trophonius::NotificationType;

    class State
    {
    private:
      std::unique_ptr<plasma::meta::Client>       _meta;
      std::unique_ptr<plasma::trophonius::Client> _trophonius;

    public:
      State();
      State(std::string const& token);
      ~State();

    public:
      void
      debug();

    ///
    /// Login & register
    ///
    private:
      std::map<std::string, User*> _users;
      User _me;
    public:
      /// Login to meta.
      void
      login(std::string const& email,
            std::string const& password);


    private:
      bool
      _logged;
    public:
      bool
      is_logged() { return this->_logged; }

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

      User const&
      get_me();

      std::string const&
      get_token();

      /// Retrieve a user by its public key.
      User const&
      user_from_public_key(std::string const& public_key);

      // Search users
      std::map<std::string, User const*>
      search_users(std::string const& text);

      /// Swaggers.

    private:
      typedef std::map<std::string, User const*> SwaggersMap;
      SwaggersMap _swaggers;
      bool _swaggers_dirty;

    public:
      SwaggersMap const&
      swaggers();

      User const&
      swagger(std::string const& id);

      /// Connect to trophonius
      void
      connect();

      /// Send message to user @id via trophonius
      void
      send_message(std::string const& recipient_id,
                   std::string const& message);

      void
      pull_notifications(int count, int offset);

      void
      notifications_read();

      std::string
      invite_user(std::string const& email);


    private:
      typedef std::map<std::string, plasma::Transaction> TransactionsMap;
      std::unique_ptr<TransactionsMap> _transactions;

    public:
      /// @brief Pull transactions from serveur.
      TransactionsMap const&
      transactions();

      /// @brief Get data from a specific transaction.
      Transaction const&
      transaction(std::string const& transaction_id);

    public:
      /// @brief Send a file list to a specified user.
      ///
      /// Create a network, copy files locally, create transaction.
      void
      send_files(std::string const& recipient_id_or_email,
                 std::unordered_set<std::string> const& files);

      /// @brief Update transaction status.
      ///
      /// Used to answer a transaction (accept or deny).
      void
      update_transaction(std::string const& transaction_id,
                         gap_TransactionStatus status);

      /// @brief Start the transfer process on recipient.
      ///
      void
      _download_files(std::string const& transaction_id);

    private:
      std::string _output_dir;

    public:
      void
      output_dir(std::string const& dir);

      std::string
      output_dir();

    private:
      // Functions callback on each status (set and get).

      /// @brief Use to accept the transaction for the recipient.
      void
      _accept_transaction(Transaction const& transaction);

      /// @brief Use to add rights on network when the recipient accepts.
      void
      _on_transaction_accepted(Transaction const& transaction);

      /// @brief Use to deny the transaction for the recipient.
      void
      _deny_transaction(Transaction const& transaction);

      /// @brief Use to "delete" the transaction if the recipient denied it.
      void
      _on_transaction_denied(Transaction const& transaction);

      /// @brief Use to cancel a pending transaction or an unfinished one.
      void
      _cancel_transaction(Transaction const& transaction);

      /// @brief Use to destroy network if transaction has been canceled.
      void
      _on_transaction_canceled(Transaction const& transaction);

      /// @brief Use to inform recipient that everything is ok and he can start
      /// downloading.
      void
      _start_transaction(Transaction const& transaction);

      /// @brief Use to .
      void
      _on_transaction_started(Transaction const& transaction);

      /// @brief Use to inform the sender that download is complete.
      void
      _close_transaction(Transaction const& transaction);

      /// @brief Use to close network.
      void
      _on_transaction_closed(Transaction const& transaction);


    private:
      std::string _device_id;
      std::string _device_name;

    public:
      std::string const&
      device_id();
      std::string const&
      device_name();

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
      /// XXX: old
      void
      deprecated_set_permissions(std::string const& user_id,
                                 std::string const& abspath,
                                 nucleus::neutron::Permissions permissions,
                                 bool recursive = false);

      /// Give the recipient the write on the root of the network.
      void
      set_permissions(std::string const& user_id,
                      std::string const& network_id,
                      nucleus::neutron::Permissions permissions);
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

      /// Delete a new network.
      std::string
      delete_network(std::string const& name,
                     bool force = false);

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

    private:
      typedef
        std::function<void(Notification const&, bool)>
        NotificationHandler;
      std::map<NotificationType, std::list<NotificationHandler>> _notification_handlers;

    public:
      typedef
        std::function<void(UserStatusNotification const&)>
        UserStatusNotificationCallback;

      typedef
        std::function<void(TransactionNotification const&, bool)>
        TransactionNotificationCallback;


      typedef
        std::function<void(TransactionStatusNotification const&, bool)>
        TransactionStatusNotificationCallback;

      typedef
        std::function<void(MessageNotification const&)>
        MessageNotificationCallback;

    public:
      void
      user_status_callback(UserStatusNotificationCallback const& cb);

      void
      transaction_callback(TransactionNotificationCallback const& cb);

      void
      transaction_status_callback(TransactionStatusNotificationCallback const& cb);

      void
      message_callback(MessageNotificationCallback const& cb);

    private:
      void
      _on_transaction(TransactionNotification const& notif,
                      bool is_new);

      void
      _on_transaction_status(TransactionStatusNotification const& notif);

    public:
      void
      _on_user_status_callback(UserStatusNotification const &notif);

      void
      _on_login_impl(UserStatusNotification notif);

    public:
      size_t
      poll(size_t max = 10);

    private:
      void
      _handle_notification(Notification const& notif,
                           bool _new = true);

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
