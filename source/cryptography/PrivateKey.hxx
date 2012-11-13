#ifndef INFINIT_CRYPTOGRAPHY_PRIVATEKEY_HXX
# define INFINIT_CRYPTOGRAPHY_PRIVATEKEY_HXX

# include <cryptography/finally.hh>
# include <cryptography/bn.hh>

# include <elle/Buffer.hh>
# include <elle/idiom/Open.hh> // XXX

# include <openssl/rsa.h>

namespace infinit
{
  namespace cryptography
  {

    template <typename T>
    T
    PrivateKey::decrypt(Code const& code) const
    {
      Clear clear{this->decrypt(code)};
      T value;

      clear.buffer().reader() >> value;

      return (value);
    }

    template<typename T>  elle::Status
      PrivateKey::Encrypt(T const& in, Code& out) const
      {
        static_assert(
            !std::is_same<T, elle::Buffer>::value,
            "explicit cast to WeakBuffer needed"
        );

        elle::Buffer buf;

        try
          {
            buf.writer() << in;
          }
        catch (std::exception const& err)
          {
            escape("Cannot save object: %s", err.what());
          }

        return this->Encrypt(
            elle::WeakBuffer(buf),
            out
        );
      }

    template <typename T>
    Signature
    PrivateKey::sign(T const& plain) const
    {
      assert((!std::is_same<T, elle::Buffer>::value));

      elle::Buffer buffer;

      buffer.writer() << plain;

      return (this->sign(elle::WeakBuffer(buffer)));
    }

  }
}

//
// ---------- serialize -------------------------------------------------------
//

# include <elle/serialize/Serializer.hh>

# include <cryptography/cryptography.hh>

ELLE_SERIALIZE_SPLIT(infinit::cryptography::PrivateKey)

ELLE_SERIALIZE_SPLIT_SAVE(infinit::cryptography::PrivateKey,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);

  enforce(value._key != nullptr);

  archive << *value._key->pkey.rsa->n;
  archive << *value._key->pkey.rsa->e;
  archive << *value._key->pkey.rsa->d;
  archive << *value._key->pkey.rsa->p;
  archive << *value._key->pkey.rsa->q;
  archive << *value._key->pkey.rsa->dmp1;
  archive << *value._key->pkey.rsa->dmq1;
  archive << *value._key->pkey.rsa->iqmp;
}

ELLE_SERIALIZE_SPLIT_LOAD(infinit::cryptography::PrivateKey,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);

  ::BIGNUM *n = ::BN_new();
  ::BIGNUM *e = ::BN_new();
  ::BIGNUM *d = ::BN_new();
  ::BIGNUM *p = ::BN_new();
  ::BIGNUM *q = ::BN_new();
  ::BIGNUM *dmp1 = ::BN_new();
  ::BIGNUM *dmq1 = ::BN_new();
  ::BIGNUM *iqmp = ::BN_new();

  CRYPTOGRAPHY_FINALLY_FREE_BN(n);
  CRYPTOGRAPHY_FINALLY_FREE_BN(e);
  CRYPTOGRAPHY_FINALLY_FREE_BN(d);
  CRYPTOGRAPHY_FINALLY_FREE_BN(p);
  CRYPTOGRAPHY_FINALLY_FREE_BN(q);
  CRYPTOGRAPHY_FINALLY_FREE_BN(dmp1);
  CRYPTOGRAPHY_FINALLY_FREE_BN(dmq1);
  CRYPTOGRAPHY_FINALLY_FREE_BN(iqmp);

  archive >> *n
          >> *e
          >> *d
          >> *p
          >> *q
          >> *dmp1
          >> *dmq1
          >> *iqmp;

  CRYPTOGRAPHY_FINALLY_ABORT(n);
  CRYPTOGRAPHY_FINALLY_ABORT(e);
  CRYPTOGRAPHY_FINALLY_ABORT(d);
  CRYPTOGRAPHY_FINALLY_ABORT(p);
  CRYPTOGRAPHY_FINALLY_ABORT(q);
  CRYPTOGRAPHY_FINALLY_ABORT(dmp1);
  CRYPTOGRAPHY_FINALLY_ABORT(dmq1);
  CRYPTOGRAPHY_FINALLY_ABORT(iqmp);

  value._construct(n, e, d, p, q, dmp1, dmq1, iqmp);

  ELLE_ASSERT(value._key != nullptr);
}

#endif
