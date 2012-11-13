#ifndef  SURFACE_GAP_GAP_H
# define SURFACE_GAP_GAP_H

# ifdef __cplusplus
extern "C" {
# endif

    /// Status returned from gap calls.
    typedef enum
    {
      gap_ok = 0,
      gap_error = -1,
      gap_network_error = -2,
      gap_internal_error = -3,
      gap_no_device_error = -4,
      gap_not_logged_in = -5,
      gap_api_error = -1000,
      gap_api_error_not_log_in = -1001,
      gap_file_not_found = -2000,
      gap_no_file = 2001,
      gap_wrong_passport = 3001,
      gap_bad_request = -9,
      gap_already_logged_in = -10,
      gap_email_not_valid = -500,
      gap_handle_not_valid = -501,
      gap_device_not_valid = -502,
      gap_password_not_valid = -503,
      gap_user_id_not_valid = -504,
      gap_network_id_not_valid = -505,
      gap_device_id_not_valid = -506,
      gap_field_is_empty = -507,
      gap_activation_code_not_valid = -508,
      gap_deprecated = -888,
      gap_email_already_registred = -10003,
      gap_handle_already_registred = -10005,
      gap_device_already_registred = -10008,
      gap_activation_code_doesnt_exist = -10009,
      gap_email_password_dont_match = -10101,
      gap_unknown_user = -10201,
      gap_user_already_in_network = -20001,
      gap_network_not_found = -20002,
      gap_device_not_found = -20003,
      gap_device_not_in_network = -20004,
      gap_root_block_already_exist = -20005,
      gap_root_block_badly_signed = -20006,
      gap_user_already_invited = -30001,
      gap_user_already_in_infinit = -30002,
      gap_file_name_empty = -40000,
      gap_unknown = -666666,
    } gap_Status;

    typedef struct
    {
      char const* _id;
      char const* fullname;
      char const* email;
      char const* public_key;
    } gap_User;

    /// gap_State is an opaque structure used in every calls.
    typedef void gap_State;

    /// - gap ctor & dtor -----------------------------------------------------

    /// Create a new state.
    /// Returns NULL on failure.
    gap_State* gap_new();    /// Release a state.
    void gap_free(gap_State* state);

    /// Enable debug messages.
    void gap_enable_debug(gap_State* state);

    /// Scratch db
    gap_Status gap_debug(gap_State* state);

    /// - Services status -----------------------------------------------------

    /// Check if meta is alive.
    gap_Status gap_meta_status(gap_State* state);

    /// Debug func: Pull notifications.
    gap_Status
    gap_meta_pull_notification(gap_State* state,
                               int limit);

    gap_Status
    gap_meta_notifications_red(gap_State*);

    /// - Authentication & registration ---------------------------------------

    /// Generate a hash for the password.
    /// NOTE: You are responsible to free the returned pointer with
    /// gap_hash_free.
    char* gap_hash_password(gap_State* state,
                            char const* email,
                            char const* password);

    /// Free a previously allocated hash.
    void gap_hash_free(char* h);

    /// Login to meta.
    gap_Status gap_login(gap_State* state,
                         char const* email,
                         char const* hash_password);

    /// Logout from meta.
    gap_Status gap_logout(gap_State* state);

    /// Register to meta. If the device name is not NULL, it will also create
    /// the local device with specified name. The password hash is obtained via
    /// gap_hash_password() function.
    gap_Status gap_register(gap_State* state,
                            char const* fullname,
                            char const* email,
                            char const* hash_password,
                            char const* device_name,
                            char const* activation_code);

    /// Login to trophonius
    gap_Status
    gap_trophonius_connect(gap_State* state);

    /// - Trophonius ----------------------------------------------------------

    typedef enum
    {
      gap_notification_debug = 0,
      gap_notification_user_status,
      gap_notification_file_transfer_request,
      gap_notification_file_transfer_status,
      gap_notification_message,
    } gap_Notification;


  // XXX: inherite is_new ?
    ////////////////////////////////
    // Login/Logout/AFK/... Notification
    typedef struct
    {
      const char* user_id;
      int status;
    } gap_UserStatusNotification;

    typedef void (*gap_user_status_callback_t)(gap_UserStatusNotification const*);

    gap_Status
    gap_user_status_callback(gap_State* state,
                             gap_user_status_callback_t cb);

    ////////////////////////////////
    // File transfer recieved callback.
    typedef struct
    {
      const char* first_filename;
      int files_count;
      int total_size;
      int is_directory;
      const char* network_id;
      const char* sender_id;
      const char* sender_fullname;
      const char* transaction_id;
      int is_new;
    } gap_TransactionNotification;

    typedef void (*gap_transaction_callback_t)(gap_TransactionNotification const*);

    gap_Status
    gap_transaction_callback(gap_State* state,
                             gap_transaction_callback_t cb);

    ////////////////////////////////
    // File transfer status callback.
    typedef struct
    {
      const char* transaction_id;
      const char* network_id;
      const char* sender_device_id;
      const char* recipient_device_id;
      const char* recipient_device_name;
      int status;
      int is_new;
    } gap_TransactionStatusNotification;

    typedef void (*gap_transaction_status_callback_t)(gap_TransactionStatusNotification const*);

    gap_Status
    gap_transaction_status_callback(gap_State* state,
                                    gap_transaction_status_callback_t cb);

    ////////////////////////////////
    // Chat message.
    typedef struct
    {
      const char* sender_id;
      /* FIXME: remove that shit */
      const char* recipient_id;
      const char* message;
      int is_new;
    } gap_MessageNotification;

    typedef void (*gap_message_callback_t)(gap_MessageNotification const*);

    gap_Status
    gap_message_callback(gap_State* state,
                           gap_message_callback_t cb);

    ////////////////////////////////
    // Bite callback.
    typedef struct
    {
      char const* debug;
      int is_new;
    } gap_BiteNotification;

    // Poll
    gap_Status
    gap_poll(gap_State* state);

    gap_Status
    gap_send_files(gap_State* state,
                  char const* recipient_id,
                  char const* const* files);

    gap_Status
    gap_update_transaction(gap_State* state,
                           char const* transaction_id,
                           int status);


    gap_Status
    gap_start_transaction(gap_State* state,
                          char const* transaction_id);

    gap_Status
    gap_invite_user(gap_State* state,
                    char const* email);

    gap_Status
    gap_message(gap_State* state,
                char const* recipient_id,
                char const* message);

    /// - Device --------------------------------------------------------------
    /// Returns the local device status.
    gap_Status gap_device_status(gap_State* state);

    /// Update the local device name.
    gap_Status gap_set_device_name(gap_State* state,
                                   char const* name);

    /// - Network -------------------------------------------------------------

    /// Create a new network.
    gap_Status gap_create_network(gap_State* state,
                                  char const* name);

    /// Retrieve all user networks ids. Returned value is null in case of
    /// error, or is a null-terminated array of null-terminated strings.
    char** gap_networks(gap_State* state);

    /// Release the pointer returned by gap_networks,
    void gap_networks_free(char** networks);

    /// Get the network name from its id.
    char const* gap_network_name(gap_State* state, char const* id);

    /// Get the network mount point.
    char const* gap_network_mount_point(gap_State* state,
                                        char const* id);

    /// Retreive all users ids from a network. Returns a null-terminated array
    /// of null-terminated strings.
    char** gap_network_users(gap_State* state, char const* id);

    /// Free a previously allocated network users'.
    void gap_network_users_free(char** users);

    /// Invite a user to join a network with its id or email.
    gap_Status gap_network_add_user(gap_State* state,
                                    char const* network_id,
                                    char const* user_id);

    /// - User ----------------------------------------------------------------

    /// Retrieve user fullname.
    char const* gap_user_fullname(gap_State* state, char const* id);

    /// Retrieve user email address.
    char const* gap_user_email(gap_State* state, char const* id);

    /// Retrieve user with its email.
    char const* gap_user_by_email(gap_State* state, char const* email);

    /// Search users.
    char** gap_search_users(gap_State* state, char const* text);

    /// Free the search users result.
    void gap_search_users_free(char** users);


  //void free_user(gap_User*)

    /// - Watchdog ------------------------------------------------------------

    /// Launch the watchdog binary.
    gap_Status gap_launch_watchdog(gap_State* state);

    /// Notify the watchdog that networks has changed.
    gap_Status gap_refresh_networks(gap_State* state);

    /// Stop the watchdog process.
    gap_Status gap_stop_watchdog(gap_State* state);

    /// - Permissions ---------------------------------------------------------

    typedef enum gap_Permission
    {
      gap_none  = 0,
      gap_read  = 1,
      gap_write = 2,
      gap_exec  = 4,
      // WARNING: negative values are reserved for errors, no value of this
      // enum should have a negative value.
    } gap_Permission;

    /// Change file permissions for a user.
    // XXX exec permission does not work
    gap_Status gap_set_permissions(gap_State* state,
                                   char const* user_id,
                                   char const* absolute_path,
                                   int permissions);

    /// Retrieve users associated to a file (have read and/or write access).
    char** gap_file_users(gap_State* state,
                          char const* absolute_path);

    void gap_file_users_free(char** users);

    /// Retrieve file permissions for a user. Returns -1 in case of errors.
    // XXX exec permission does not work
    int gap_get_permissions(gap_State* state,
                            char const* user_id,
                            char const* absolute_path);

# ifdef __cplusplus
} // ! extern "C"
# endif

#endif
