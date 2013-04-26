#ifndef INFINIT_CRYPTOGRAPHY_RSA_PUBLICKEY_HXX
# define INFINIT_CRYPTOGRAPHY_RSA_PUBLICKEY_HXX

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

ELLE_SERIALIZE_SPLIT(infinit::cryptography::rsa::PublicKey)

ELLE_SERIALIZE_SPLIT_SAVE(infinit::cryptography::rsa::PublicKey,
                          archive,
                          value,
                          format)
{
  enforce(format == 0);

  ELLE_ASSERT_NEQ(value._key, nullptr);

  archive << *value._key->pkey.rsa->n
          << *value._key->pkey.rsa->e;
}

ELLE_SERIALIZE_SPLIT_LOAD(infinit::cryptography::rsa::PublicKey,
                          archive,
                          value,
                          format)
{
  enforce(format == 0);

  ::BIGNUM *n = ::BN_new();
  ::BIGNUM *e = ::BN_new();

  INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(n);
  INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(e);

  ELLE_ASSERT_NEQ(n, nullptr);
  ELLE_ASSERT_NEQ(e, nullptr);

  archive >> *n
          >> *e;

  INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(n);
  INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(e);

  value._construct(n, e);
  value._prepare();

  ELLE_ASSERT_NEQ(value._key, nullptr);
}

#endif
