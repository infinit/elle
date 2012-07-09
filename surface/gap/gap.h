#ifndef  SURFACE_GAP_GAP_H
# define SURFACE_GAP_GAP_H

# ifdef __cplusplus
extern "C" {
# endif

    /// Status returned from gap calls.
    enum gap_Status
    {
      gap_ok = 0,
      gap_error = -1,
      gap_network_error = -2,
      gap_internal_error = -3,
      gap_api_error = -4,
    };

    /// gap_State is an opaque structure used in every calls.
    typedef void gap_State;

    /// Create a new state.
    /// Returns NULL on failure.
    gap_State* gap_new();

    /// Release a state.
    void gap_free(gap_State* state);

    /// Check if meta is alive.
    gap_Status gap_meta_status(gap_State* state);

    /// Notify the watchdog that networks has changed.
    gap_Status gap_refresh_networks(gap_State* state);

    /// Retreive a network id from a path. Do not store or free the returned
    /// pointer. Just copy the string if you plan to use it later.
    /// Returns NULL on failure.
    char const* gap_path_to_network(gap_State* state,
                                    char const* path);

    /// Generate a hash for the password.
    /// NOTE: You are responsible to free the returned pointer with gap_hash_free.
    char* gap_hash_password(gap_State* state,
                            char const* email,
                            char const* password);

    /// Free a previously allocated hash.
    void gap_hash_free(char* h);

    /// Login to meta.
    gap_Status gap_login(gap_State* state,
                         char const* email,
                         char const* hash_password);

    /// Register to meta. If the device name is not NULL, it will also create
    /// the local device with specified name. password ha
    gap_Status gap_register(gap_State* state,
                            char const* fullname,
                            char const* email,
                            char const* hash_password,
                            char const* device_name);

    /// Update the local device name.
    gap_Status gap_set_device_name(gap_State* state,
                                   char const* name);

    /// Retrieve all user networks ids. Returned value is null in case of
    /// error, or is a null-terminated array of null-terminated strings.
    char** gap_networks(gap_State* state);

    /// Release the pointer returned by gap_networks,
    void gap_networks_free(char** networks);

    /// Create a new network.
    gap_Status gap_create_network(gap_State* state,
                                  char const* name);


    /// Launch the watchdog binary. If the given path is NULL, then the default
    /// binary path will be used.
    gap_Status gap_launch_watchdog(gap_State* state,
                                   char const* watchdog_path);

    /// Stop the watchdog process.
    gap_Status gap_stop_watchdog(gap_State* state);

# ifdef __cplusplus
} // ! extern "C"
# endif

#endif

