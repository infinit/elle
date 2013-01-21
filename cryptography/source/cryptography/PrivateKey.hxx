#ifndef INFINIT_CRYPTOGRAPHY_PRIVATEKEY_HXX
# define INFINIT_CRYPTOGRAPHY_PRIVATEKEY_HXX

# include <cryptography/finally.hh>
# include <cryptography/bn.hh>

# include <elle/Buffer.hh>
# include <elle/log.hh>

# include <openssl/rsa.h>

namespace infinit
{
  namespace cryptography
  {
    /*--------.
    | Methods |
    `--------*/

    template <typename T>
    T
    PrivateKey::decrypt(Code const& code) const
    {
      ELLE_LOG_COMPONENT("infinit.cryptography.PrivateKey");
      ELLE_DEBUG_FUNCTION(code);

      static_assert(!std::is_same<T, Clear>::value,
                    "this call should never have occured");

      Clear clear{this->decrypt(code)};

      // XXX[this is the way it should be] T value{clear.buffer().reader()};
      T value;
      clear.buffer().reader() >> value;

      return (value);
    }

    template <typename T>
    Signature
    PrivateKey::sign(T const& value) const
    {
      ELLE_LOG_COMPONENT("infinit.cryptography.PrivateKey");
      ELLE_DEBUG_FUNCTION(value);

      static_assert(!std::is_same<T, Plain>::value,
                    "this call should never have occured");

      elle::Buffer buffer;
      buffer.writer() << value;

      return (this->sign(Plain{elle::WeakBuffer{buffer}}));
    }

    template <typename T>
    Code
    PrivateKey::encrypt(T const& value) const
    {
      ELLE_LOG_COMPONENT("infinit.cryptography.PrivateKey");
      ELLE_DEBUG_FUNCTION(value);

      static_assert(!std::is_same<T, Plain>::value,
                    "this call should never have occured");

      elle::Buffer buffer;
      buffer.writer() << value;

      return (this->encrypt(Plain{elle::WeakBuffer{buffer}}));
    }
  }
}

/*-------------.
| Serializable |
`-------------*/

# include <elle/serialize/Serializer.hh>

# include <cryptography/finally.hh>
# include <cryptography/bn.hh>

ELLE_SERIALIZE_SPLIT(infinit::cryptography::PrivateKey)

ELLE_SERIALIZE_SPLIT_SAVE(infinit::cryptography::PrivateKey,
                          archive,
                          value,
                          format)
{
  enforce(format == 0);

  enforce(value._key != nullptr);

  archive << *value._key->pkey.rsa->n
          << *value._key->pkey.rsa->e
          << *value._key->pkey.rsa->d
          << *value._key->pkey.rsa->p
          << *value._key->pkey.rsa->q
          << *value._key->pkey.rsa->dmp1
          << *value._key->pkey.rsa->dmq1
          << *value._key->pkey.rsa->iqmp;
}

ELLE_SERIALIZE_SPLIT_LOAD(infinit::cryptography::PrivateKey,
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

  archive >> *n
          >> *e
          >> *d
          >> *p
          >> *q
          >> *dmp1
          >> *dmq1
          >> *iqmp;

  INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(n);
  INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(e);
  INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(d);
  INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(p);
  INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(q);
  INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(dmp1);
  INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(dmq1);
  INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(iqmp);

  value._construct(n, e, d, p, q, dmp1, dmq1, iqmp);

  ELLE_ASSERT(value._key != nullptr);
}

#endif
