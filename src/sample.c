#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dopenssl/rsa.h>
#include <dopenssl/rand.h>

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    printf("usage: %s [passphrase]\n", argv[0]);
    return(1);
  }

  unsigned int const bits = 2048;

  char const* passphrase = argv[1];
  unsigned int const length = strlen(passphrase);
  unsigned int const size = bits / 8;
  unsigned int const occurences = size / length + 1;

  unsigned char* seed = malloc(occurences * length + 1);
  memset(seed, 0x0, occurences * length + 1);

  /* Concatenate the passphrase so as to create a 256-byte buffer */
  unsigned int i;
  for (i = 0; i < occurences; i++)
    strncpy(seed + (i * length), passphrase, length);

  /* Initialize the deterministic random generator */
  if (dRAND_init() != 1)
  {
    printf("[error] unable to initialize the random generator\n");
    return (1);
  }

  /* Generate the RSA key */
  RSA* rsa = NULL;

  if ((rsa = dRSA_deduce_privatekey(bits, seed, size)) == NULL)
  {
    printf("[error] unable to generate the RSA key pair\n");
    return (1);
  }

  /* Display the RSA components */
  RSA_print_fp(stdout, rsa, 0);

  /* Clean the resources */
  if (dRAND_clean() != 1)
  {
    printf("[error] unable to clean the random generator\n");
    return (1);
  }

  return (0);
}
