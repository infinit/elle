#include <openssl/engine.h>
#include <openssl/crypto.h>
#include <openssl/dsa.h>
#include <openssl/err.h>
#include <openssl/pem.h>

#include <elle/Error.hh>
#include <elle/finally.hh>
#include <elle/log.hh>
#include <elle/printf.hh>

#include <cryptography/dsa/PublicKey.hh>
#include <cryptography/dsa/PrivateKey.hh>
#include <cryptography/dsa/KeyPair.hh>
#include <cryptography/dsa/der.hh>
#include <cryptography/dsa/serialization.hh>
#include <cryptography/context.hh>
#include <cryptography/low.hh>
#include <cryptography/Digest.hh>
#include <cryptography/Code.hh>
#include <cryptography/Exception.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/bn.hh>
#include <cryptography/finally.hh>
#include <cryptography/evp.hh>
#include <cryptography/hash.hh>

ELLE_LOG_COMPONENT("infinit.cryptography.dsa.PublicKey");

//
// ---------- Class -----------------------------------------------------------
//

namespace infinit
{
  namespace cryptography
  {
    namespace dsa
    {
      namespace publickey
      {
        /*--------------.
        | Serialization |
        `--------------*/

        struct Serialization:
          public dsa::serialization::DSA
        {
          static
          elle::Buffer
          encode(::DSA* dsa)
          {
            return der::encode_public(dsa);
          }

          static
          ::DSA*
          decode(elle::ConstWeakBuffer const& buffer)
          {
            return der::decode_public(buffer);
          }
        };
      }

      /*-------------.
      | Construction |
      `-------------*/

      PublicKey::PublicKey(PrivateKey const& k)
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Set the key parameters.
        this->_digest_algorithm = k.digest_algorithm();

        // Extract the public key only.
        DSA* _dsa = low::DSA_priv2pub(k.key().get()->pkey.dsa);

        ELLE_ASSERT_NEQ(_dsa->pub_key, nullptr);
        ELLE_ASSERT_EQ(_dsa->priv_key, nullptr);

        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_DSA(_dsa);

        // Construct the public key based on the given DSA structure whose
        // ownership is retained.
        this->_construct(_dsa);

        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(_dsa);

        // Prepare the cryptographic contexts.
        this->_prepare();

        this->_check();
      }

      PublicKey::PublicKey(::EVP_PKEY* key,
                           Oneway const digest_algorithm):
        _key(key),
        _digest_algorithm(digest_algorithm)
      {
        ELLE_ASSERT_NEQ(key, nullptr);
        ELLE_ASSERT_NEQ(key->pkey.dsa->pub_key, nullptr);
        ELLE_ASSERT_EQ(key->pkey.dsa->priv_key, nullptr);

        // Make sure the cryptographic system is set up.
        cryptography::require();

        if (::EVP_PKEY_type(this->_key->type) != EVP_PKEY_DSA)
          throw Exception(
            elle::sprintf("the EVP_PKEY key is not of type DSA: %s",
                          ::EVP_PKEY_type(this->_key->type)));

        // Prepare the cryptographic contexts.
        this->_prepare();

        this->_check();
      }

      PublicKey::PublicKey(::DSA* dsa,
                           Oneway const digest_algorithm):
        _digest_algorithm(digest_algorithm)
      {
        ELLE_ASSERT_NEQ(dsa, nullptr);
        ELLE_ASSERT_NEQ(dsa->pub_key, nullptr);
        ELLE_ASSERT_EQ(dsa->priv_key, nullptr);

        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Construct the public key based on the given DSA structure.
        this->_construct(dsa);

        // Prepare the cryptographic contexts.
        this->_prepare();

        this->_check();
      }

      PublicKey::PublicKey(PublicKey const& other):
        _digest_algorithm(other._digest_algorithm)
      {
        ELLE_ASSERT_NEQ(other._key->pkey.dsa->pub_key, nullptr);

        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Duplicate the DSA structure.
        DSA* _dsa = low::DSA_dup(other._key->pkey.dsa);

        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_DSA(_dsa);

        this->_construct(_dsa);

        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(_dsa);

        this->_prepare();

        this->_check();
      }

      PublicKey::PublicKey(PublicKey&& other):
        _key(std::move(other._key)),
        _digest_algorithm(std::move(other._digest_algorithm))
      {
        this->_context.verify = std::move(other._context.verify);

        // Make sure the cryptographic system is set up.
        cryptography::require();

        this->_check();
      }

      /*--------.
      | Methods |
      `--------*/

      void
      PublicKey::_construct(::DSA* dsa)
      {
        ELLE_DEBUG_FUNCTION(dsa);

        ELLE_ASSERT_NEQ(dsa, nullptr);

        // Initialise the public key structure.
        ELLE_ASSERT_EQ(this->_key, nullptr);
        this->_key.reset(::EVP_PKEY_new());

        if (this->_key == nullptr)
          throw Exception(
            elle::sprintf("unable to allocate the EVP_PKEY structure: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        // Set the dsa structure into the public key.
        if (::EVP_PKEY_assign_DSA(this->_key.get(), dsa) <= 0)
          throw Exception(
            elle::sprintf("unable to assign the DSA key to the EVP_PKEY "
                          "structure: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));
      }

      void
      PublicKey::_prepare()
      {
        ELLE_DEBUG_FUNCTION("");

        ELLE_ASSERT_NEQ(this->_key, nullptr);

        // Prepare the verify context.
        ELLE_ASSERT_EQ(this->_context.verify, nullptr);
        this->_context.verify.reset(
          context::create(this->_key.get(),
                          ::EVP_PKEY_verify_init));
      }

      void
      PublicKey::_check() const
      {
        ELLE_ASSERT_NEQ(this->_key, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.dsa, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.dsa->pub_key, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.dsa->priv_key, nullptr);
      }

      elle::Boolean
      PublicKey::verify(Signature const& signature,
                        Plain const& plain) const
      {
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("signature: %x", signature);
        ELLE_DUMP("plain: %x", plain);

        Digest digest = hash(plain, this->_digest_algorithm);

        return (this->verify(signature, digest));
      }

      elle::Boolean
      PublicKey::verify(Signature const& signature,
                        Digest const& digest) const
      {
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("signature: %x", signature);
        ELLE_DUMP("digest: %x", digest);

        return (evp::asymmetric::verify(signature.buffer(),
                                        digest.buffer(),
                                        this->_context.verify.get(),
                                        ::EVP_PKEY_verify));
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

        return (::EVP_PKEY_cmp(this->_key.get(), other._key.get()) == 1);
      }

      /*--------------.
      | Serialization |
      `--------------*/

      PublicKey::PublicKey(elle::serialization::SerializerIn& serializer)
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Allocate the EVP key to receive the deserialized's DSA structure.
        this->_key.reset(::EVP_PKEY_new());

        // Set the EVP key as being of type DSA.
        if (::EVP_PKEY_set_type(this->_key.get(), EVP_PKEY_DSA) <= 0)
          throw Exception(
            elle::sprintf("unable to set the EVP key's type: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        this->serialize(serializer);

        this->_prepare();
        this->_check();
      }

      void
      PublicKey::serialize(elle::serialization::Serializer& serializer)
      {
        ELLE_ASSERT_NEQ(this->_key, nullptr);

        cryptography::serialize<publickey::Serialization>(
          serializer,
          this->_key->pkey.dsa);

        serializer.serialize("digest algorithm", this->_digest_algorithm);
      }

      /*----------.
      | Printable |
      `----------*/

      void
      PublicKey::print(std::ostream& stream) const
      {
        ELLE_ASSERT_NEQ(this->_key, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.dsa, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.dsa->pub_key, nullptr);

        stream << "("
               << *this->_key->pkey.dsa->pub_key
               << ")";

        stream << "["
               << this->_digest_algorithm
               << "]";
      }
    }
  }
}
