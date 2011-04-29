//
// ---------- header ----------------------------------------------------------
//
// project       comet
//
// license       infinit
//
// file          /home/mycure/infinit/comet/RSA.hh
//
// created       julien quintard   [fri mar 18 16:18:50 2011]
// updated       julien quintard   [wed mar 23 14:22:44 2011]
//

#ifndef COMET_RSA_HH
#define COMET_RSA_HH

//
// ---------- includes --------------------------------------------------------
//

#include <openssl/rsa.h>

namespace comet
{

//
// ---------- prototypes ------------------------------------------------------
//

  int RSA_rotate(RSA *rsa, int bits, const unsigned char *seed, size_t length);
  int RSA_derive(RSA *rsa, BIGNUM* N, const unsigned char *seed, size_t length);

}

#endif
