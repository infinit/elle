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

    /// Login to meta.
    gap_Status gap_login(gap_State* state,
                         char const* email,
                         char const* password);

    /// Register to meta. If the device name is not NULL, it will also create
    /// the local device with specified name.
    gap_Status gap_register(gap_State* state,
                            char const* fullname,
                            char const* email,
                            char const* password,
                            char const* device_name);

    /// Update the local device name.
    gap_Status gap_set_device_name(gap_State* state,
                                   char const* name);

# ifdef __cplusplus
} // ! extern "C"
# endif

#endif

