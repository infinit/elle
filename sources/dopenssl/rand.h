/*
 * Copyright (c) 2013, infinit.io
 *
 * This software is provided "as is" without warranty of any kind,
 * either expressed or implied, including but not limited to the
 * implied warranties of fitness for a particular purpose.
 *
 * See the LICENSE file for more information on the terms and
 * conditions.
 */

#ifndef DOPENSSL_RAND_H
# define DOPENSSL_RAND_H

# include <openssl/rand.h>

/* This structure defines the functions representing the deterministic
   implementation of a PNRG.
   In practice, these functions make sure not to directly
   add entropy neither to call functions which may do. */
extern RAND_METHOD const dRAND_method;

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
/* Display on the standard output the state of the PNRG. */
void dRAND_state(void);
/* Return a string-based representation of the PNRG's state.
   Note that it is the responsibility of the caller to release the resources
   associated wit the returned string, via free(). */
char *dRAND_fingerprint(void);

#endif
