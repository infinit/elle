#ifndef COMET_RSA_HH
# define COMET_RSA_HH

# include <openssl/rsa.h>

/* Fill the public key of the given _rsa_ with the modulus _N_ and deduce the
   exponent with a deterministic PNRG seeded with the content provided. */
RSA *dRSA_deduce_publickey(BIGNUM *N,
                           const unsigned char *seed, size_t seed_length);
/* Deduce the whole private key _rsa_ of the given _size_ in bits by relying on
   a deterministic PNRG seeded with the content provided. */
RSA *dRSA_deduce_privatekey(int bits,
                            const unsigned char *seed, size_t seed_length);
/* Compare the public part of the two given RSA keys. */
int dRSA_cmp_publickey(RSA *a, RSA *b);
/* Compare the private part of the two given RSA keys. */
int dRSA_cmp_privatekey(RSA *a, RSA *b);
/* Display on the standard output the various numbers composing an RSA key. */
int dRSA_print(RSA *rsa);

#endif
