#ifndef  SURFACE_GAP_GAP_H
# define SURFACE_GAP_GAP_H

# ifdef __cplusplus
extern "C" {
# endif

    /// gap_state_t is an opaque structure used in every calls.
    struct gap_state;
    typedef struct gap_state gap_state_t;

    /// Create a new state.
    /// Returns NULL on failure.
    // XXX The updater path is a temporary hack to launch the
    // updater binary.
    gap_state_t* gap_new(char const* updater_path);

    /// Release a state.
    void gap_free(gap_state_t* state);

    /// Prepare and update infinit binaries.
    /// Returns 0 on success, -1 on failure.
    int gap_update_infinit_home(gap_state_t* state);

    /// Notify the watchdog that networks has changed.
    /// Returns 0 on success, -1 on failure.
    int gap_refresh_networks(gap_state_t* state);

    /// Retreive a network id from a path. Do not store or free the returned
    /// pointer. Just copy the string if you plan to use it later.
    /// Returns NULL on failure.
    char const* gap_path_to_network(gap_state_t* state, char const* path);

    /// Login to meta.
    /// Returns 0 on success, -1 on failure.
    int gap_login(gap_state_t* state,
                  char const* email,
                  char const* password);

    /// Register to meta.
    /// Return 0 on success, -1 on failure.
    int gap_register(gap_state_t* state,
                     char const* fullname,
                     char const* email,
                     char const* password);

# ifdef __cplusplus
} // ! extern "C"
# endif

#endif

