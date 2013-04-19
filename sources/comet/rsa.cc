#include <comet/rsa.hh>
#include <comet/rand.hh>
#include <comet/bn.hh>

#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/bn.h>
#include <openssl/rand.h>

#include <stdio.h>
#include <assert.h>

/*
 * ---------- Additional Functionalities --------------------------------------
 *
 * Based on the following OpenSSL files:
 *
 *   crypto/rsa/rsa_gen.c
 */

RSA *dRSA_deduce_publickey(BIGNUM *N,
                           const unsigned char *seed, size_t seed_length)
{
  int bits,bitse,ok= -1;

  /* PATCHED[allocate an RSA key] */
  RSA *rsa = ::RSA_new();

  bits=BN_num_bits(N);
  bitse=(bits/2)+1;

  /* We need the RSA components non-NULL */
  if(!rsa->n && ((rsa->n=BN_new()) == NULL)) goto err;
  if(!rsa->e && ((rsa->e=BN_new()) == NULL)) goto err;

  /* Initialize the 'private' component as unknown. */
  rsa->d = NULL;
  rsa->p = NULL;
  rsa->q = NULL;
  rsa->dmp1 = NULL;
  rsa->dmq1 = NULL;
  rsa->iqmp = NULL;

  /* PATCHED[switch to our random generator in order to ensure
             determinism. note that the dRAND module must have been
             initialized] */
  dRAND_start();
  {
    assert(RAND_get_rand_method() == &dRAND_method);

    dRAND_reset();
    RAND_seed(seed, seed_length);

    /* generate e */
    /* PATCHED[here we use our prime generator which uses our deterministic
               random generator] */
    if(!dBN_generate_prime_ex(rsa->e, bitse, 0, NULL, NULL, NULL))
      goto err;
  }
  dRAND_stop();

  /* assign n */
  BN_copy(rsa->n, N);

  ok=1;
  err:
  if (ok == -1)
  {
    /* PATCHED[release the RSA structure and reinitialize the
       pointer to NULL] */
    if (rsa != NULL) RSA_free(rsa);
    rsa = NULL;

    RSAerr(RSA_F_RSA_BUILTIN_KEYGEN,ERR_LIB_BN);
  }

  return rsa;
}

RSA *dRSA_deduce_privatekey(int bits,
                            const unsigned char *seed, size_t seed_length)
{
  BIGNUM *r0=NULL,*r1=NULL,*r2=NULL,*r3=NULL,*tmp;
  BIGNUM local_r0,local_d,local_p;
  BIGNUM *pr0,*d,*p;
  int bitse,bitsp,bitsq,ok= -1;
  BN_CTX *ctx=NULL;

  /* PATCHED[allocate an RSA key] */
  RSA *rsa = ::RSA_new();

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

  /* PATCHED[switch to our random generator in order to ensure
             determinism. note that the dRAND module must have been
             initialized] */
  dRAND_start();
  {
    assert(RAND_get_rand_method() == &dRAND_method);

    dRAND_reset();
    RAND_seed(seed, seed_length);

    /* generate e */
    /* PATCHED[use our prime generator which uses our deterministic
               random generator] */
    if(!dBN_generate_prime_ex(rsa->e, bitse, 0, NULL, NULL, NULL))
      goto err;

    /* generate p and q */
    for (;;)
    {
      if(!dBN_generate_prime_ex(rsa->p, bitsp, 0, NULL, NULL, NULL))
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
        if(!dBN_generate_prime_ex(rsa->q, bitsq, 0, NULL, NULL, NULL))
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
  }
  dRAND_stop();

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
    /* PATCHED[release the RSA structure and reinitialize the
       pointer to NULL] */
    if (rsa != NULL) RSA_free(rsa);
    rsa = NULL;

    RSAerr(RSA_F_RSA_BUILTIN_KEYGEN,ERR_LIB_BN);
  }
  if (ctx != NULL)
  {
    BN_CTX_end(ctx);
    BN_CTX_free(ctx);
  }

  return rsa;
}

int dRSA_cmp_publickey(RSA *a, RSA *b)
{
  int x;

  assert(a->n != NULL);
  assert(a->e != NULL);
  assert(b->n != NULL);
  assert(b->e != NULL);

  if ((x = BN_cmp(a->n, b->n)) != 0)
    return x;
  if ((x = BN_cmp(a->e, b->e)) != 0)
    return x;

  return 0;
}

int dRSA_cmp_privatekey(RSA *a, RSA *b)
{
  int x;

  assert(a->n != NULL);
  assert(a->e != NULL);
  assert(a->d != NULL);
  assert(a->p != NULL);
  assert(a->q != NULL);
  assert(a->dmp1 != NULL);
  assert(a->dmq1 != NULL);
  assert(a->iqmp != NULL);
  assert(b->n != NULL);
  assert(b->e != NULL);
  assert(b->d != NULL);
  assert(b->p != NULL);
  assert(b->q != NULL);
  assert(b->dmp1 != NULL);
  assert(b->dmq1 != NULL);
  assert(b->iqmp != NULL);

  if ((x = BN_cmp(a->n, b->n)) != 0)
    return x;
  if ((x = BN_cmp(a->e, b->e)) != 0)
    return x;
  if ((x = BN_cmp(a->d, b->d)) != 0)
    return x;
  if ((x = BN_cmp(a->p, b->p)) != 0)
    return x;
  if ((x = BN_cmp(a->q, b->q)) != 0)
    return x;
  if ((x = BN_cmp(a->dmp1, b->dmp1)) != 0)
    return x;
  if ((x = BN_cmp(a->dmq1, b->dmq1)) != 0)
    return x;
  if ((x = BN_cmp(a->iqmp, b->iqmp)) != 0)
    return x;

  return 0;
}

int dRSA_print(RSA *rsa)
{
  char *n = NULL;
  char *e = NULL;
  char *d = NULL;
  char *p = NULL;
  char *q = NULL;
  char *dmp1 = NULL;
  char *dmq1 = NULL;
  char *iqmp = NULL;

  assert(rsa->n != NULL);
  assert(rsa->e != NULL);

  if ((n = BN_bn2hex(rsa->n)) == NULL)
    return 0;
  if ((e = BN_bn2hex(rsa->e)) == NULL)
    return 0;
  if (rsa->d != NULL)
    if ((d = BN_bn2hex(rsa->d)) == NULL)
      return 0;
  if (rsa->p != NULL)
    if ((p = BN_bn2hex(rsa->p)) == NULL)
      return 0;
  if (rsa->q != NULL)
    if ((q = BN_bn2hex(rsa->q)) == NULL)
      return 0;
  if (rsa->dmp1 != NULL)
    if ((dmp1 = BN_bn2hex(rsa->dmp1)) == NULL)
      return 0;
  if (rsa->dmq1 != NULL)
    if ((dmq1 = BN_bn2hex(rsa->dmq1)) == NULL)
      return 0;
  if (rsa->iqmp != NULL)
    if ((iqmp = BN_bn2hex(rsa->iqmp)) == NULL)
      return 0;

  printf("n=%s e=%s d=%s p=%s q=%s dmp1=%s dmq1=%s iqmp=%s\n",
         n, e, d, p, q, dmp1, dmq1, iqmp);

  OPENSSL_free(iqmp);
  OPENSSL_free(dmq1);
  OPENSSL_free(dmp1);
  OPENSSL_free(q);
  OPENSSL_free(p);
  OPENSSL_free(d);
  OPENSSL_free(n);
  OPENSSL_free(e);

  return 1;
}
