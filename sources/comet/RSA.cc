#include <comet/RSA.hh>
#include <comet/RAND.hh>
#include <comet/BN.hh>

#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/bn.h>
#include <openssl/rand.h>

#include <stdio.h>
#include <time.h>

namespace comet
{

  RAND_METHOD specific =
    {
      RAND_seed,
      RAND_bytes,
      RAND_cleanup,
      RAND_add,
      RAND_pseudorand,
      RAND_status
    };

  int RSA_rotate(RSA *rsa, int bits, const unsigned char *seed, size_t length)
  {
    const RAND_METHOD *generic;
    BIGNUM *r0=NULL,*r1=NULL,*r2=NULL,*r3=NULL,*tmp;
    BIGNUM local_r0,local_d,local_p;
    BIGNUM *pr0,*d,*p;
    int bitse,bitsp,bitsq,ok= -1;
    BN_CTX *ctx=NULL;

    ctx=BN_CTX_new();
    if (ctx == NULL) goto err;
    BN_CTX_start(ctx);
    r0 = BN_CTX_get(ctx);
    r1 = BN_CTX_get(ctx);
    r2 = BN_CTX_get(ctx);
    r3 = BN_CTX_get(ctx);
    if (r3 == NULL) goto err;

    bitse=(bits/2)+1;
    bitsp=(bits+1)/2;
    bitsq=bits-bitsp;

    /* We need the RSA components non-NULL */
    if(!rsa->n && ((rsa->n=BN_new()) == NULL)) goto err;
    if(!rsa->d && ((rsa->d=BN_new()) == NULL)) goto err;
    if(!rsa->e && ((rsa->e=BN_new()) == NULL)) goto err;
    if(!rsa->p && ((rsa->p=BN_new()) == NULL)) goto err;
    if(!rsa->q && ((rsa->q=BN_new()) == NULL)) goto err;
    if(!rsa->dmp1 && ((rsa->dmp1=BN_new()) == NULL)) goto err;
    if(!rsa->dmq1 && ((rsa->dmq1=BN_new()) == NULL)) goto err;
    if(!rsa->iqmp && ((rsa->iqmp=BN_new()) == NULL)) goto err;

    /// switch to our random generator in order to ensure
    /// determinism: (i) save the current random generator
    /// (ii) switch to ours (iii) clean it in order to
    /// ensure it is reset (iv) seed it with the given buffer.
    generic = RAND_get_rand_method();

    RAND_set_rand_method(&specific);
    RAND_cleanup();
    RAND_set_rand_method(&specific);

    RAND_seed(seed, length);

    /* generate e */
    /// here we use our prime generator which uses our deterministic
    /// random generator.
    if(!comet::BN_generate_prime_ex(rsa->e, bitse, 0, NULL, NULL, NULL))
      goto err;

    /// switch back to the OpenSSL random generator.
    RAND_set_rand_method(generic);

    /* generate p and q */
    for (;;)
      {
        if(!::BN_generate_prime_ex(rsa->p, bitsp, 0, NULL, NULL, NULL))
          goto err;
        if (!BN_sub(r2,rsa->p,BN_value_one())) goto err;
        if (!BN_gcd(r1,r2,rsa->e,ctx)) goto err;
        if (BN_is_one(r1)) break;
      }
    for (;;)
      {
        /* When generating ridiculously small keys, we can get stuck
         * continually regenerating the same prime values. Check for
         * this and bail if it happens 3 times. */
        unsigned int degenerate = 0;
                do
                  {
                    if(!::BN_generate_prime_ex(rsa->q, bitsq, 0, NULL, NULL, NULL))
                      goto err;
                  } while((BN_cmp(rsa->p, rsa->q) == 0) && (++degenerate < 3));
                if(degenerate == 3)
                  {
                    ok = 0; /* we set our own err */
                    RSAerr(RSA_F_RSA_BUILTIN_KEYGEN,RSA_R_KEY_SIZE_TOO_SMALL);
                    goto err;
                  }
                if (!BN_sub(r2,rsa->q,BN_value_one())) goto err;
                if (!BN_gcd(r1,r2,rsa->e,ctx)) goto err;
                if (BN_is_one(r1))
                  break;
      }
    if (BN_cmp(rsa->p,rsa->q) < 0)
      {
        tmp=rsa->p;
        rsa->p=rsa->q;
        rsa->q=tmp;
      }

    /* calculate n */
    if (!BN_mul(rsa->n,rsa->p,rsa->q,ctx)) goto err;

    /* calculate d */
    if (!BN_sub(r1,rsa->p,BN_value_one())) goto err;        /* p-1 */
    if (!BN_sub(r2,rsa->q,BN_value_one())) goto err;        /* q-1 */
    if (!BN_mul(r0,r1,r2,ctx)) goto err;    /* (p-1)(q-1) */
    if (!(rsa->flags & RSA_FLAG_NO_CONSTTIME))
      {
        pr0 = &local_r0;
        BN_with_flags(pr0, r0, BN_FLG_CONSTTIME);
      }
    else
      pr0 = r0;
    if (!BN_mod_inverse(rsa->d,rsa->e,pr0,ctx)) goto err;   /* d */

    /* set up d for correct BN_FLG_CONSTTIME flag */
    if (!(rsa->flags & RSA_FLAG_NO_CONSTTIME))
      {
        d = &local_d;
        BN_with_flags(d, rsa->d, BN_FLG_CONSTTIME);
      }
    else
      d = rsa->d;

    /* calculate d mod (p-1) */
    if (!BN_mod(rsa->dmp1,d,r1,ctx)) goto err;

    /* calculate d mod (q-1) */
    if (!BN_mod(rsa->dmq1,d,r2,ctx)) goto err;

    /* calculate inverse of q mod p */
    if (!(rsa->flags & RSA_FLAG_NO_CONSTTIME))
      {
        p = &local_p;
        BN_with_flags(p, rsa->p, BN_FLG_CONSTTIME);
      }
    else
      p = rsa->p;
    if (!BN_mod_inverse(rsa->iqmp,rsa->q,p,ctx)) goto err;

    ok=1;
  err:
    if (ok == -1)
      {
        RSAerr(RSA_F_RSA_BUILTIN_KEYGEN,ERR_LIB_BN);
        ok=0;
      }
    if (ctx != NULL)
      {
        BN_CTX_end(ctx);
        BN_CTX_free(ctx);
      }

    return ok;
  }

  int RSA_derive(RSA *rsa, BIGNUM* N, const unsigned char *seed, size_t length)
  {
    const RAND_METHOD *generic;
    int bits,bitse,ok= -1;

    bits=BN_num_bits(N);
    bitse=(bits/2)+1;

    /* We need the RSA components non-NULL */
    if(!rsa->n && ((rsa->n=BN_new()) == NULL)) goto err;
    if(!rsa->e && ((rsa->e=BN_new()) == NULL)) goto err;

    /* initialize the 'private' component as unknown. */
    rsa->d = NULL;
    rsa->d = NULL;
    rsa->p = NULL;
    rsa->q = NULL;
    rsa->dmp1 = NULL;
    rsa->dmq1 = NULL;
    rsa->iqmp = NULL;

    /// switch to our random generator in order to ensure
    /// determinism: (i) save the current random generator
    /// (ii) switch to ours (iii) clean it in order to
    /// ensure it is reset (iv) seed it with the given buffer.
    generic = RAND_get_rand_method();

    RAND_set_rand_method(&specific);
    RAND_cleanup();
    RAND_set_rand_method(&specific);

    RAND_seed(seed, length);

    /* generate e */
    /// here we use our prime generator which uses our deterministic
    /// random generator.
    if(!comet::BN_generate_prime_ex(rsa->e, bitse, 0, NULL, NULL, NULL))
      goto err;

    /// switch back to the OpenSSL random generator.
    RAND_set_rand_method(generic);

    /* assign n */
    BN_copy(rsa->n, N);

    ok=1;
  err:
    if (ok == -1)
      {
        RSAerr(RSA_F_RSA_BUILTIN_KEYGEN,ERR_LIB_BN);
        ok=0;
      }

    return ok;
  }

}
