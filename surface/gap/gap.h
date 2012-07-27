#ifndef  SURFACE_GAP_GAP_H
# define SURFACE_GAP_GAP_H

# ifdef __cplusplus
extern "C" {
# endif

    /// Status returned from gap calls.
    typedef enum gap_Status
    {
      gap_ok = 0,
      gap_error = -1,
      gap_network_error = -2,
      gap_internal_error = -3,
      gap_api_error = -4,
      gap_no_device_error = -5,
      gap_not_logged_in = -6,
    } gap_Status;

    /// gap_State is an opaque structure used in every calls.
    typedef void gap_State;

    /// - gap ctor & dtor -----------------------------------------------------

    /// Create a new state.
    /// Returns NULL on failure.
    gap_State* gap_new();

    /// Release a state.
    void gap_free(gap_State* state);

    /// - Services status -----------------------------------------------------

    /// Check if meta is alive.
    gap_Status gap_meta_status(gap_State* state);


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

