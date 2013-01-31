#ifndef  SURFACE_GAP_STATE_HH
# define SURFACE_GAP_STATE_HH

# include "gap.h"
# include "_detail/InfinitInstanceManager.hh"

# include <nucleus/neutron/Permissions.hh>

# include <plasma/meta/Client.hh>
# include <plasma/trophonius/Client.hh>

# include <elle/format/json/fwd.hh>

# include <reactor/scheduler.hh> // XXX

# include <functional>
# include <map>
# include <string>
# include <unordered_set>

# define CATCH_FAILURE_TO_METRICS(prefix)                               \
  catch (elle::HTTPException const& e)                                  \
  {                                                                     \
    metrics::google::server().store(prefix  ":fail",                    \
                                    "cd1", "http" + std::to_string((int) e.code)); \
    throw;                                                              \
  }                                                                     \
  catch (surface::gap::Exception const& e)                              \
  {                                                                     \
    metrics::google::server().store(prefix ":fail",                     \
                                    "cd1", "gap" + std::to_string((int) e.code)); \
    throw;                                                              \
  }                                                                     \
  catch (...)                                                           \
  {                                                                     \
    metrics::google::server().store(prefix ":fail",                     \
                                    "cd1", "unknown");                  \
    throw;                                                              \
  } /* */


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
    using User = ::plasma::meta::User;
    using Nodes = ::plasma::meta::NetworkNodesResponse;
    using Network = ::plasma::meta::NetworkResponse;
    using Endpoint = ::plasma::meta::EndpointNodeResponse;


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
    using ::plasma::trophonius::NetworkUpdateNotification;
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

      void
      output_log_file(std::string const& path);

    //- Login & register ------------------------------------------------------
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
      logged_in() const
      { return !this->_meta->token().empty(); }

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

      elle::Buffer
      user_icon(std::string const& id);


      /// Retrieve current user data.
      User const&
      get_me();

      /// Retrieve current user token.
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

      /// @brief Fetch transaction from server (and update it).
      Transaction const&
      transaction_sync(std::string const& id);

      /// @brief Returns a floating number in [0.0f, 1.0f]
      float
      transaction_progress(std::string const& transaction_id);

    public:
      /// A process is indexed with a unique identifier.
      typedef size_t ProcessId;

      /// The status of a process. failure or success implies that the process
      /// is terminated.
      enum class ProcessStatus : int { failure = 0, success = 1, running = 2};
    private:
      struct Process;
      typedef std::unique_ptr<Process> ProcessPtr;
      typedef std::unordered_map<ProcessId, ProcessPtr> ProcessMap;
      ProcessMap _processes;

    public:
      /// Retreive the status of an existing process.
      ProcessStatus
      process_status(ProcessId const id) const;

      /// @brief Remove a process and throw the exception if any.
      ///
      /// @throw if the process does not exist or if it is still running.
      void process_finalize(ProcessId const id);

    private:
      ProcessId
      _add_process(std::string const& name,
                   std::function<void(void)> const& cb);

    public:
      /// @brief Send a file list to a specified user.
      ///
      /// Create a network, copy files locally, create transaction.
      ProcessId
      send_files(std::string const& recipient_id_or_email,
                 std::unordered_set<std::string> const& files);
    private:
      void
      _send_files(std::string const& recipient,
                  std::unordered_set<std::string> const& files);

    public:
      /// @brief Update transaction status.
      ///
      /// Used to answer a transaction (accept or deny).
      void
      update_transaction(std::string const& transaction_id,
                         gap_TransactionStatus status);

    private:
      /// @brief Start the transfer process on recipient.
      ///
      void
      _download_files(std::string const& transaction_id);

    private:
      std::string _output_dir;

    public:
      void
      output_dir(std::string const& dir);

      std::string const&
      output_dir() const;

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

      /// @brief Use to inform recipient that everything is ok and he can prepare
      /// downloading.
      void
      _prepare_transaction(Transaction const& transaction);

      /// @brief Use to .
      void
      _on_transaction_prepared(Transaction const& transaction);

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
      file_size(std::string const& path);

      std::string
      file_name(std::string const& path);

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
      typedef std::unique_ptr<Network> NetworkPtr;
      std::map<std::string, NetworkPtr>     _networks;
      bool                                  _networks_dirty;
      typedef std::unique_ptr<InfinitInstanceManager> InfinitInstanceManagerPtr;
      InfinitInstanceManagerPtr             _infinit_instance_manager;

    public:
      InfinitInstanceManager&
      infinit_instance_manager();

    private:
      bool
      _wait_portal(std::string const& network_id);

    public:
      /// Retrieve all networks.
      std::map<std::string, NetworkPtr> const&
      networks();

      /// Retrieve a network.
      Network&
      network(std::string const& id);

      /// Create a new network.
      std::string
      create_network(std::string const& name);

      /// Prepare directories and files for the network to be launched.
      void
      prepare_network(std::string const& network_id);

      /// Delete a new network.
      std::string
      delete_network(std::string const& name,
                     bool force = false);

      /// Add a user to a network with its mail or id.
      void
      network_add_user(std::string const& network_id,
                       std::string const& user);

    private:
      void
      _create_network_root_block(std::string const& id);

      void
      _prepare_network_directory(std::string const& network_id);

    private:
      typedef
        std::function<void(Notification const&, bool)>
        NotificationHandler;
      typedef
        std::map<NotificationType, std::list<NotificationHandler>>
        NotificationHandlerMap;
      NotificationHandlerMap  _notification_handlers;

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

      typedef
        std::function<void(NetworkUpdateNotification const&)>
        NetworkUpdateNotificationCallback;

    public:
      void
      user_status_callback(UserStatusNotificationCallback const& cb);

      void
      transaction_callback(TransactionNotificationCallback const& cb);

      void
      transaction_status_callback(TransactionStatusNotificationCallback const& cb);

      void
      message_callback(MessageNotificationCallback const& cb);

      void
      network_update_callback(NetworkUpdateNotificationCallback const& cb);

    private:
      void
      _on_transaction(TransactionNotification const& notif,
                      bool is_new);

      void
      _on_transaction_status(TransactionStatusNotification const& notif);

      void
      _on_network_update(NetworkUpdateNotification const& notif);

      void
      _on_user_status_update(UserStatusNotification const& notif);

    public:
      void
      _notify_8infinit(Transaction const& trans,
                       reactor::Scheduler& sched);

    public:
      size_t
      poll(size_t max = 10);

    private:
      void
      _handle_notification(Notification const& notif,
                           bool _new = true);

    };

  }
}


#endif
