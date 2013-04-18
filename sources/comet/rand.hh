#ifndef COMET_RAND_HH
# define COMET_RAND_HH

# include <openssl/rand.h>

/* The functions below represent the deterministic
   implementation of a PNRG.
   In practice, these functions make sure not to directly
   add entropy neither to call functions which may do. */

extern RAND_METHOD dRAND_method;
void dRAND_cleanup(void);
void dRAND_add(const void *buf, int num, double add);
void dRAND_seed(const void *buf, int num);
int dRAND_bytes(unsigned char *buf, int num);
int dRAND_pseudorand(unsigned char *buf, int num);
int dRAND_status(void);

/* Initialize the deterministic PNRG by setting up a new cryptographic
   ENGINE. */
int dRAND_init();
/* Clean the dRAND implementation from the list of engines. */
int dRAND_clean();
/* Register the dRAND implementation as the default one, ensuring that all
   RAND_* operations will be forwarded to the dRAND engine. */
int dRAND_start(void);
/* Unregister the dRAND engine as the default implementation. */
int dRAND_stop(void);
/* Reset the random implementation so as to make sure that PNRG is in its
   initial state.
   This function is particularly useful before seeding the PNRG assuming the
   caller expects the PNRG's state to be deterministic. */
int dRAND_reset(void);
/* Return a string-based representation of the PNRG's state. */
char *dRAND_fingerprint(void);

#endif
