#ifndef COMET_RAND_HH
# define COMET_RAND_HH

# include <openssl/rand.h>

extern RAND_METHOD dRAND_method;

void dRAND_cleanup(void);
void dRAND_add(const void *buf, int num, double add);
void dRAND_seed(const void *buf, int num);
int dRAND_bytes(unsigned char *buf, int num);
int dRAND_pseudorand(unsigned char *buf, int num);
int dRAND_status(void);

void dRAND_start(void);
void dRAND_stop(void);
void dRAND_reset(void);
char* dRAND_fingerprint(void);

#endif
