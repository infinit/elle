#ifndef INFINIT_CRYPTOGRAPHY_PUBLICKEY_HXX
# define INFINIT_CRYPTOGRAPHY_PUBLICKEY_HXX

# include <elle/Buffer.hh>

namespace infinit
{
  namespace cryptography
  {
    /*--------.
    | Methods |
    `--------*/

    template <typename T>
    Code
    PublicKey::encrypt(T const& value) const
    {
      ELLE_LOG_COMPONENT("infinit.cryptography.PublicKey");
      ELLE_DEBUG_FUNCTION(value);

      static_assert(!std::is_same<T, Plain>::value,
                    "this call should never have occured");

      elle::Buffer buffer;
      buffer.writer() << value;

      return (this->encrypt(Plain{elle::WeakBuffer{buffer}}));
    }

    template <typename T>
    T
    PublicKey::decrypt(Code const& code) const
    {
      ELLE_LOG_COMPONENT("infinit.cryptography.PublicKey");
      ELLE_DEBUG_FUNCTION(code);

      static_assert(!std::is_same<T, Clear>::value,
                    "this call should never have occured");

      Clear clear = this->decrypt(code);

      T value;
      code.buffer().reader() >> value;

      return (value);
    }

    template <typename T>
    elle::Boolean
    PublicKey::verify(Signature const& signature,
                      T const& value) const
    {
      ELLE_LOG_COMPONENT("infinit.cryptography.PublicKey");
      ELLE_DEBUG_FUNCTION(signature, value);

      static_assert(!std::is_same<T, Plain>::value,
                    "this call should never have occured");

      elle::Buffer buffer;
      buffer.writer() << value;

      return (this->verify(signature, Plain{elle::WeakBuffer{buffer}}));
    }
  }
}

/*-------------.
| Serializable |
`-------------*/

# include <elle/serialize/Serializer.hh>

# include <cryptography/finally.hh>
# include <cryptography/bn.hh>

ELLE_SERIALIZE_SPLIT(infinit::cryptography::PublicKey)

ELLE_SERIALIZE_SPLIT_SAVE(infinit::cryptography::PublicKey,
                          archive,
                          value,
                          format)
{
  enforce(format == 0);

  enforce(value._key != nullptr);

  archive << *value._key->pkey.rsa->n
          << *value._key->pkey.rsa->e;
}

ELLE_SERIALIZE_SPLIT_LOAD(infinit::cryptography::PublicKey,
                          archive,
                          value,
                          format)
{
  enforce(format == 0);

  ::BIGNUM *n = ::BN_new();
  ::BIGNUM *e = ::BN_new();

  INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(n);
  INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(e);

  archive >> *n
          >> *e;

  INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(n);
  INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(e);

  value._construct(n, e);

  ELLE_ASSERT(value._key != nullptr);
}

#endif
