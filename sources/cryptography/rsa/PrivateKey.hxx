#ifndef INFINIT_CRYPTOGRAPHY_RSA_PRIVATEKEY_HXX
# define INFINIT_CRYPTOGRAPHY_RSA_PRIVATEKEY_HXX

//
// ---------- Class -----------------------------------------------------------
//

/*-------------.
| Serializable |
`-------------*/

# include <elle/serialize/Serializer.hh>

# include <cryptography/finally.hh>
# include <cryptography/bn.hh>

# include <openssl/rsa.h>

ELLE_SERIALIZE_SPLIT(infinit::cryptography::rsa::PrivateKey)

ELLE_SERIALIZE_SPLIT_SAVE(infinit::cryptography::rsa::PrivateKey,
                          archive,
                          value,
                          format)
{
  enforce(format == 0);

  ELLE_ASSERT_NEQ(value._key, nullptr);

  archive << *value._key->pkey.rsa->n
          << *value._key->pkey.rsa->e
          << *value._key->pkey.rsa->d
          << *value._key->pkey.rsa->p
          << *value._key->pkey.rsa->q
          << *value._key->pkey.rsa->dmp1
          << *value._key->pkey.rsa->dmq1
          << *value._key->pkey.rsa->iqmp;
}

ELLE_SERIALIZE_SPLIT_LOAD(infinit::cryptography::rsa::PrivateKey,
                          archive,
                          value,
                          format)
{
  enforce(format == 0);

  ::BIGNUM *n = ::BN_new();
  ::BIGNUM *e = ::BN_new();
  ::BIGNUM *d = ::BN_new();
  ::BIGNUM *p = ::BN_new();
  ::BIGNUM *q = ::BN_new();
  ::BIGNUM *dmp1 = ::BN_new();
  ::BIGNUM *dmq1 = ::BN_new();
  ::BIGNUM *iqmp = ::BN_new();

  INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(n);
  INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(e);
  INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(d);
  INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(p);
  INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(q);
  INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(dmp1);
  INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(dmq1);
  INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(iqmp);

  ELLE_ASSERT_NEQ(n, nullptr);
  ELLE_ASSERT_NEQ(e, nullptr);
  ELLE_ASSERT_NEQ(d, nullptr);
  ELLE_ASSERT_NEQ(p, nullptr);
  ELLE_ASSERT_NEQ(q, nullptr);
  ELLE_ASSERT_NEQ(dmp1, nullptr);
  ELLE_ASSERT_NEQ(dmq1, nullptr);
  ELLE_ASSERT_NEQ(iqmp, nullptr);

  archive >> *n
          >> *e
          >> *d
          >> *p
          >> *q
          >> *dmp1
          >> *dmq1
          >> *iqmp;

  value._construct(n, e, d, p, q, dmp1, dmq1, iqmp);

  INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(n);
  INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(e);
  INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(d);
  INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(p);
  INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(q);
  INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(dmp1);
  INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(dmq1);
  INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(iqmp);

  value._prepare();

  ELLE_ASSERT_NEQ(value._key, nullptr);
}

#endif
