#ifndef  SURFACE_GAP_GAP_H
# define SURFACE_GAP_GAP_H

# ifdef __cplusplus
extern "C" {
# endif

    struct gap_state;
    typedef struct gap_state gap_state_t;

    gap_state_t* gap_new();
    void gap_free(gap_state_t* state);

    int gap_refresh_networks(gap_state_t* state);

    char const* gap_path_to_network(gap_state_t* state, char const* path);

    int gap_login(gap_state_t* state,
                  char const* email,
                  char const* password);

# ifdef __cplusplus
} // ! extern "C"
# endif

#endif

