#include <openssl/engine.h>
#include <openssl/crypto.h>
#include <openssl/rsa.h>
#include <openssl/err.h>

#include <elle/Error.hh>
#include <elle/finally.hh>
#include <elle/log.hh>
#include <elle/printf.hh>

#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
# include <dopenssl/rsa.hh>
#endif

#include <cryptography/rsa/PublicKey.hh>
#include <cryptography/rsa/PrivateKey.hh>
#include <cryptography/rsa/Seed.hh>
#include <cryptography/rsa/padding.hh>
#include <cryptography/rsa/context.hh>
#include <cryptography/Digest.hh>
#include <cryptography/Code.hh>
#include <cryptography/Exception.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/bn.hh>
#include <cryptography/evp.hh>

ELLE_LOG_COMPONENT("infinit.cryptography.rsa.PublicKey");

//
// ---------- Class -----------------------------------------------------------
//

namespace infinit
{
  namespace cryptography
  {
    namespace rsa
    {
      /*-------------.
      | Construction |
      `-------------*/

      PublicKey::PublicKey()
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();
      }

      PublicKey::PublicKey(::EVP_PKEY* key):
        _key(key)
      {
        ELLE_ASSERT_NEQ(key, nullptr);
        ELLE_ASSERT_NEQ(key->pkey.rsa->n, nullptr);
        ELLE_ASSERT_NEQ(key->pkey.rsa->e, nullptr);
        ELLE_ASSERT_EQ(key->pkey.rsa->d, nullptr);
        ELLE_ASSERT_EQ(key->pkey.rsa->p, nullptr);
        ELLE_ASSERT_EQ(key->pkey.rsa->q, nullptr);
        ELLE_ASSERT_EQ(key->pkey.rsa->dmp1, nullptr);
        ELLE_ASSERT_EQ(key->pkey.rsa->dmq1, nullptr);
        ELLE_ASSERT_EQ(key->pkey.rsa->iqmp, nullptr);

        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Prepare the cryptographic contexts.
        this->_prepare();

        ELLE_ASSERT_NEQ(this->_key, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa->n, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa->e, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->d, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->p, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->q, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->dmp1, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->dmq1, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->iqmp, nullptr);
      }

      PublicKey::PublicKey(::RSA* rsa)
      {
        ELLE_ASSERT_NEQ(rsa, nullptr);
        ELLE_ASSERT_NEQ(rsa->n, nullptr);
        ELLE_ASSERT_NEQ(rsa->e, nullptr);
        ELLE_ASSERT_EQ(rsa->d, nullptr);
        ELLE_ASSERT_EQ(rsa->p, nullptr);
        ELLE_ASSERT_EQ(rsa->q, nullptr);
        ELLE_ASSERT_EQ(rsa->dmp1, nullptr);
        ELLE_ASSERT_EQ(rsa->dmq1, nullptr);
        ELLE_ASSERT_EQ(rsa->iqmp, nullptr);

        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Construct the public key based on the given RSA structure.
        this->_construct(rsa);

        // Prepare the cryptographic contexts.
        this->_prepare();

        ELLE_ASSERT_NEQ(this->_key, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa->n, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa->e, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->d, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->p, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->q, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->dmp1, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->dmq1, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->iqmp, nullptr);
      }

      PublicKey::PublicKey(::BIGNUM* n,
                           ::BIGNUM* e)
      {
        ELLE_ASSERT_NEQ(n, nullptr);
        ELLE_ASSERT_NEQ(e, nullptr);

        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Call a private method for constructing the object so as
        // to also be callable from the serialization mechanism, especially
        // the deserialization.
        this->_construct(n, e);

        // Prepare the cryptographic contexts.
        this->_prepare();

        ELLE_ASSERT_NEQ(this->_key, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa->n, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa->e, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->d, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->p, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->q, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->dmp1, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->dmq1, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->iqmp, nullptr);
      }

      PublicKey::PublicKey(PublicKey const& other):
        elle::serialize::DynamicFormat<PublicKey>(other)
      {
        ELLE_ASSERT_NEQ(other._key->pkey.rsa->n, nullptr);
        ELLE_ASSERT_NEQ(other._key->pkey.rsa->e, nullptr);

        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Call a private method for constructing the object so as
        // to also be callable from the serialization mechanism, especially
        // the deserialization.
        this->_construct(::BN_dup(other._key->pkey.rsa->n),
                         ::BN_dup(other._key->pkey.rsa->e));

        // Prepare the cryptographic contexts.
        this->_prepare();

        ELLE_ASSERT_NEQ(this->_key, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa->n, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa->e, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->d, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->p, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->q, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->dmp1, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->dmq1, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->iqmp, nullptr);
      }

      PublicKey::PublicKey(PublicKey&& other):
        elle::serialize::DynamicFormat<PublicKey>(std::move(other)),
        _key(std::move(other._key)),
        _context_encrypt(std::move(other._context_encrypt)),
        _context_encrypt_padding_size(other._context_encrypt_padding_size),
        _context_verify(std::move(other._context_verify)),
        _context_decrypt(std::move(other._context_decrypt))
#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
        , _context_rotate(std::move(other._context_rotate))
        , _context_derive(std::move(other._context_derive))
#endif
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();

        ELLE_ASSERT_EQ(other._key, nullptr);
        ELLE_ASSERT_EQ(other._context_encrypt, nullptr);
        ELLE_ASSERT_EQ(other._context_verify, nullptr);
        ELLE_ASSERT_EQ(other._context_decrypt, nullptr);
#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
        ELLE_ASSERT_EQ(other._context_rotate, nullptr);
        ELLE_ASSERT_EQ(other._context_derive, nullptr);
#endif
      }

      ELLE_SERIALIZE_CONSTRUCT_DEFINE(PublicKey)
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();

        // XXX would need to prepare

        ELLE_ASSERT_NEQ(this->_key, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa->n, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa->e, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->d, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->p, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->q, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->dmp1, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->dmq1, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.rsa->iqmp, nullptr);
      }

      /*--------.
      | Methods |
      `--------*/

      void
      PublicKey::_construct(::RSA* rsa)
      {
        ELLE_DEBUG_FUNCTION(rsa);

        ELLE_ASSERT_NEQ(rsa, nullptr);

        // Initialise the public key structure.
        ELLE_ASSERT_EQ(this->_key, nullptr);
        this->_key.reset(::EVP_PKEY_new());

        if (this->_key == nullptr)
          throw Exception(
            elle::sprintf("unable to allocate the EVP_PKEY structure: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        // Set the rsa structure into the public key.
        if (::EVP_PKEY_assign_RSA(this->_key.get(), rsa) <= 0)
          throw Exception(
            elle::sprintf("unable to assign the RSA key to the EVP_PKEY "
                          "structure: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));
      }

      void
      PublicKey::_construct(::BIGNUM* n,
                            ::BIGNUM* e)
      {
        ELLE_DEBUG_FUNCTION(n, e);

        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(n);
        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(e);

        ::RSA* rsa = nullptr;

        // Create the RSA structure.
        if ((rsa = ::RSA_new()) == nullptr)
          throw Exception(
            elle::sprintf("unable to allocate the RSA structure: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_RSA(rsa);

        // Assign the big numbers relevant to the public key.
        rsa->n = n;
        rsa->e = e;

        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(n);
        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(e);

        // Construct the public key based on an RSA key.
        this->_construct(rsa);

        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(rsa);
      }

      void
      PublicKey::_prepare()
      {
        ELLE_DEBUG_FUNCTION("");

        ELLE_ASSERT_NEQ(this->_key, nullptr);

        // Depending on the format from which the public key has been
        // serialized or created, set the padding for the contexts.
        int padding_encrypt;
        int padding_verify;
        int padding_decrypt;

        switch (this->version())
        {
          case 0:
          {
            padding_encrypt = RSA_PKCS1_OAEP_PADDING;
            padding_verify = RSA_PKCS1_PADDING;
            padding_decrypt = RSA_PKCS1_PADDING;

            break;
          }
          case 1:
          {
            padding_encrypt = RSA_PKCS1_PADDING;
            padding_verify = RSA_PKCS1_PADDING;
            padding_decrypt = RSA_PKCS1_PADDING;

            break;
          }
          case 2:
          {
            padding_encrypt = RSA_PKCS1_OAEP_PADDING;
            padding_verify = RSA_PKCS1_PSS_PADDING;
            padding_decrypt = RSA_PKCS1_PADDING;

            break;
          }
          default:
            throw Exception(
              elle::sprintf("unknown format '%s'", this->version()));
        }

        // Prepare the encrypt context.
        ELLE_ASSERT_EQ(this->_context_encrypt, nullptr);
        this->_context_encrypt.reset(
          context::create(this->_key.get(),
                          ::EVP_PKEY_encrypt_init,
                          padding_encrypt));

        this->_context_encrypt_padding_size =
          padding::footprint(this->_context_encrypt.get());

        // Prepare the verify context.
        ELLE_ASSERT_EQ(this->_context_verify, nullptr);
        this->_context_verify.reset(
          context::create(this->_key.get(),
                          ::EVP_PKEY_verify_init,
                          padding_verify));

        switch (this->version())
        {
          case 0:
          case 1:
          {
            break;
          }
          case 2:
          {
            // Configure the PSS signature-specific padding.
            if (::EVP_PKEY_CTX_ctrl(this->_context_verify.get(),
                                    EVP_PKEY_RSA,
                                    -1,
                                    EVP_PKEY_CTRL_RSA_PSS_SALTLEN,
                                    -2,
                                    nullptr) <= 0)
              throw Exception(
                elle::sprintf("unable to set the EVP_PKEY context's PSS "
                              "salt length: %s",
                              ::ERR_error_string(ERR_get_error(), nullptr)));

            // Set the digest function.
            if (::EVP_PKEY_CTX_ctrl(this->_context_verify.get(),
                                    EVP_PKEY_RSA,
                                    EVP_PKEY_OP_TYPE_SIG,
                                    EVP_PKEY_CTRL_MD,
                                    0,
                                    (void*)::EVP_sha256()) <= 0)
              throw Exception(
                elle::sprintf("unable to set the EVP_PKEY context's digest "
                              "function: %s",
                              ::ERR_error_string(ERR_get_error(), nullptr)));

            break;
          }
          default:
            throw Exception(
              elle::sprintf("unknown format '%s'", this->version()));
        }

        // Prepare the decrypt context.
        ELLE_ASSERT_EQ(this->_context_decrypt, nullptr);
        this->_context_decrypt.reset(
          context::create(this->_key.get(),
                          ::EVP_PKEY_verify_recover_init,
                          padding_decrypt));

        switch (this->version())
        {
          case 0:
          case 1:
          {
            break;
          }
          case 2:
          {
            // Set the digest function.
            /* XXX
            if (::EVP_PKEY_CTX_ctrl(this->_context_decrypt.get(),
                                    EVP_PKEY_RSA,
                                    EVP_PKEY_OP_TYPE_SIG,
                                    EVP_PKEY_CTRL_MD,
                                    0,
                                    (void*)::EVP_sha256()) <= 0)
              throw Exception(
                elle::sprintf("unable to set the EVP_PKEY context's digest "
                              "function: %s",
                              ::ERR_error_string(ERR_get_error(), nullptr)));
            */
            break;
          }
          default:
            throw Exception(
              elle::sprintf("unknown format '%s'", this->version()));
        }

#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
        // These contexts do not use paddings. Not that relying on textbook
        // RSA is considered foolish. In this case however, restricting the
        // rotation/derivation to content of the size of the RSA key's modulus
        // makes it secure.

        // Prepare the rotation context.
        ELLE_ASSERT_EQ(this->_context_rotate, nullptr);
        this->_context_rotate.reset(
          context::create(this->_key.get(),
                          ::EVP_PKEY_encrypt_init,
                          RSA_NO_PADDING));

        // Prepare the derive context.
        ELLE_ASSERT_EQ(this->_context_derive, nullptr);
        this->_context_derive.reset(
          context::create(this->_key.get(),
                          ::EVP_PKEY_verify_recover_init,
                          RSA_NO_PADDING));
#endif
      }

      /*----------.
      | Operators |
      `----------*/

      elle::Boolean
      PublicKey::operator ==(PublicKey const& other) const
      {
        if (this == &other)
          return (true);

        ELLE_ASSERT_NEQ(this->_key, nullptr);
        ELLE_ASSERT_NEQ(other._key, nullptr);

        // Compare the internal numbers.
        if ((::BN_cmp(this->_key->pkey.rsa->n,
                      other._key->pkey.rsa->n) != 0) ||
            (::BN_cmp(this->_key->pkey.rsa->e,
                      other._key->pkey.rsa->e) != 0))
          return (false);

        return (true);
      }

      elle::Boolean
      PublicKey::operator <(PublicKey const& other) const
      {
        if (this == &other)
          return (true);

        ELLE_ASSERT_NEQ(this->_key, nullptr);
        ELLE_ASSERT_NEQ(other._key, nullptr);

        // Compare the internal numbers.
        int cmp_n = ::BN_cmp(this->_key->pkey.rsa->n,
                             other._key->pkey.rsa->n);

        if (cmp_n < 0)
          return (true);
        else if (cmp_n > 0)
          return (false);

        int cmp_e = ::BN_cmp(this->_key->pkey.rsa->e,
                             other._key->pkey.rsa->e);

        if (cmp_e < 0)
          return (true);
        else
          return (false);

        elle::unreachable();
      }

      /*-----------.
      | Public Key |
      `-----------*/

      elle::Boolean
      PublicKey::operator ==(
        cryptography::publickey::Interface const& other) const
      {
        if (this == &other)
          return (true);

        if (this->cryptosystem() != other.cryptosystem())
          return (false);

        return (*this == static_cast<PublicKey const&>(other));
      }

      elle::Boolean
      PublicKey::operator <(
        cryptography::publickey::Interface const& other) const
      {
        if (this == &other)
          return (true);

        if (this->cryptosystem() != other.cryptosystem())
          return (this->cryptosystem() < other.cryptosystem());

        return (*this < static_cast<PublicKey const&>(other));
      }

      cryptography::publickey::Interface*
      PublicKey::clone() const
      {
        return (new PublicKey(*this));
      }

      elle::Natural32
      PublicKey::size() const
      {
        return (static_cast<elle::Natural32>(
                  ::EVP_PKEY_size(this->_key.get())));
      }

      elle::Natural32
      PublicKey::length() const
      {
        return (static_cast<elle::Natural32>(
                  ::EVP_PKEY_bits(this->_key.get())));
      }

      Cryptosystem
      PublicKey::cryptosystem() const
      {
        return (Cryptosystem::rsa);
      }

      Code
      PublicKey::encrypt(Plain const& plain) const
      {
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("plain: %x", plain);

        return (evp::asymmetric::encrypt(plain,
                                         this->_context_encrypt.get(),
                                         ::EVP_PKEY_encrypt,
                                         this->_context_encrypt_padding_size));
      }

      elle::Boolean
      PublicKey::verify(Signature const& signature,
                        Digest const& digest) const
      {
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("signature: %x", signature);
        ELLE_DUMP("digest: %x", digest);

        return (evp::asymmetric::verify(signature,
                                        digest,
                                        this->_context_verify.get(),
                                        ::EVP_PKEY_verify));
      }

      Clear
      PublicKey::decrypt(Code const& code) const
      {
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("code: %x", code);

        return (evp::asymmetric::decrypt(code,
                                         this->_context_decrypt.get(),
                                         ::EVP_PKEY_verify_recover));
      }

#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
      cryptography::Seed
      PublicKey::rotate(cryptography::Seed const& seed) const
      {
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("seed: %x", seed);

        ELLE_ASSERT_EQ(seed.cryptosystem(), Cryptosystem::rsa);

        // Cast the seed into an actual RSA seed.
        ELLE_ASSERT_NEQ(dynamic_cast<Seed const*>(&seed.implementation()),
                        nullptr);
        Seed const& _seed = static_cast<Seed const&>(seed.implementation());

        // Ensure the size of the seed equals the modulus.
        //
        // If the seed is too large, the algorithm would need to encrypt
        // it with a symmetric key etc. (as the encrypt() method does) which
        // would result in a future seed larger than the original.
        //
        // If it is too small, an attack could be performed against textbook
        // RSA which is the algorithm used in this case.
        if (_seed.length() != this->length())
          throw Exception(
            elle::sprintf("unable to rotate a seed whose length does not match "
                          "the RSA key's modulus: %s versus %s",
                          _seed.length(), this->length()));

        elle::Buffer buffer =
          evp::asymmetric::apply(elle::WeakBuffer{_seed.buffer()},
                                 this->_context_rotate.get(),
                                 ::EVP_PKEY_encrypt);

        // Make sure the seed does not grow over time.
        ELLE_ASSERT_EQ(_seed.buffer().size(), buffer.size());

        // Create an implementation of an RSA seed.
        std::unique_ptr<cryptography::seed::Interface> implementation(
          new Seed(std::move(buffer), _seed.length()));

        return (cryptography::Seed(std::move(implementation)));
      }

      cryptography::Seed
      PublicKey::derive(cryptography::Seed const& seed) const
      {
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("seed: %x", seed);

        ELLE_ASSERT_EQ(seed.cryptosystem(), Cryptosystem::rsa);

        // Cast the seed into an actual RSA seed.
        ELLE_ASSERT_NEQ(dynamic_cast<Seed const*>(&seed.implementation()),
                        nullptr);
        Seed const& _seed = static_cast<Seed const&>(seed.implementation());

        // As for the rotation mechanism, ensure the size of the seed
        // equals the modulus.
        if (_seed.length() != this->length())
          throw Exception(
            elle::sprintf("unable to derive a seed whose length does not match "
                          "the RSA key's modulus: %s versus %s",
                          _seed.length(), this->length()));

        elle::Buffer buffer =
          evp::asymmetric::apply(elle::WeakBuffer{_seed.buffer()},
                                 this->_context_derive.get(),
                                 ::EVP_PKEY_verify_recover);

        // Make sure the derived seed has the same size as the original.
        ELLE_ASSERT_EQ(_seed.buffer().size(), buffer.size());

        // Create an implementation of an RSA seed.
        std::unique_ptr<cryptography::seed::Interface> implementation(
          new Seed(std::move(buffer), _seed.length()));

        return (cryptography::Seed(std::move(implementation)));
      }
#endif

      /*--------------.
      | Serialization |
      `--------------*/

      PublicKey::PublicKey(elle::serialization::SerializerIn& serializer)
        : _key(::EVP_PKEY_new())
      {
        auto rsa = ::RSA_new();
        if (!rsa)
          throw elle::Error(
            elle::sprintf("unable to initialize RSA: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));
        if (::EVP_PKEY_assign_RSA(this->_key.get(), rsa) <= 0)
          throw elle::Error(
            elle::sprintf("unable to assign the RSA: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));
        this->serialize(serializer);
        this->_prepare();
      }

      void
      PublicKey::serialize(elle::serialization::Serializer& serializer)
      {
        serializer.serialize("modulus", this->_key->pkey.rsa->n);
        serializer.serialize("public_exponent", this->_key->pkey.rsa->e);
      }

      static const elle::serialization::Hierarchy
      <cryptography::publickey::Interface>::
      Register<PublicKey> _register_PublicKey("rsa");

      /*----------.
      | Printable |
      `----------*/

      void
      PublicKey::print(std::ostream& stream) const
      {
        ELLE_ASSERT_NEQ(this->_key, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa->n, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.rsa->e, nullptr);

        stream << "("
               << *this->_key->pkey.rsa->n
               << ", "
               << *this->_key->pkey.rsa->e
               << ")";

        stream << "[";
        padding::print(stream, this->_context_encrypt.get());
        stream << ", ";
        padding::print(stream, this->_context_verify.get());
        stream << ", ";
        padding::print(stream, this->_context_decrypt.get());
#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
        stream << ", ";
        padding::print(stream, this->_context_rotate.get());
        stream << ", ";
        padding::print(stream, this->_context_derive.get());
#endif
        stream << "]";
      }
    }
  }
}

//
// ---------- Generator -------------------------------------------------------
//

namespace infinit
{
  namespace cryptography
  {
    namespace rsa
    {
      namespace publickey
      {
#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
        PublicKey
        deduce(cryptography::seed::Interface const& seed)
        {
          ELLE_TRACE_FUNCTION("");
          ELLE_DUMP("seed: %x", seed);

          // Make sure the cryptographic system is set up.
          cryptography::require();

          ELLE_ASSERT_EQ(seed.cryptosystem(), Cryptosystem::rsa);

          // Cast the interface into an actual RSA seed.
          ELLE_ASSERT_NEQ(dynamic_cast<Seed const*>(&seed), nullptr);
          Seed const& _seed = static_cast<Seed const&>(seed);

          // Deduce the RSA key from the given seed.
          ::RSA* rsa = nullptr;

          if ((rsa = ::dRSA_deduce_privatekey(
                 _seed.length(),
                 static_cast<unsigned char const*>(_seed.buffer().contents()),
                 _seed.buffer().size())) == nullptr)
            throw Exception(
              elle::sprintf("unable to deduce the RSA key from the given "
                            "seed: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_RSA(rsa);

          // Instanciate an RSA public key by duplicating only the elements
          // of interest in the RSA structure.
          PublicKey K(::BN_dup(rsa->n), ::BN_dup(rsa->e));

          INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(rsa);

          ::RSA_free(rsa);

          return (K);
        }
#endif

        PublicKey
        construct(::EVP_PKEY* key)
        {
          INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_EVP_PKEY(key);

          // Instanciate an RSA public key by transferring the ownership
          // of the EVP structure.
          PublicKey K(key);

          INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(key);

          return (K);
        }
      }
    }
  }
}
