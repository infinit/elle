//
// ---------- header ----------------------------------------------------------
//
// project       comet
//
// license       infinit
//
// file          /home/mycure/infinit/comet/BN.hh
//
// created       julien quintard   [fri mar 18 16:39:24 2011]
// updated       julien quintard   [tue mar 22 15:09:48 2011]
//

#ifndef COMET_BNL_HH
#define COMET_BN_HH

//
// ---------- includes --------------------------------------------------------
//

#include <openssl/bn.h>

namespace comet
{

//
// ---------- prototypes ------------------------------------------------------
//

  int BN_generate_prime_ex(BIGNUM *ret, int bits, int safe,
			   const BIGNUM *add, const BIGNUM *rem, BN_GENCB *cb);

}

#endif
