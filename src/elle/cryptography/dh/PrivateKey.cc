#include <openssl/engine.h>
#include <openssl/crypto.h>
#include <openssl/dh.h>
#include <openssl/err.h>

#include <elle/log.hh>

#include <elle/cryptography/Error.hh>
#include <elle/cryptography/bn.hh>
#include <elle/cryptography/cryptography.hh>
#include <elle/cryptography/dh/KeyPair.hh>
#include <elle/cryptography/dh/PrivateKey.hh>
#include <elle/cryptography/dh/PublicKey.hh>
#include <elle/cryptography/dh/low.hh>
#include <elle/cryptography/finally.hh>
#include <elle/cryptography/hash.hh>
#include <elle/cryptography/raw.hh>

namespace elle
{
  namespace cryptography
  {
    namespace dh
    {
      /*-------------.
      | Construction |
      `-------------*/

      PrivateKey::PrivateKey(::EVP_PKEY* key):
        _key(key)
      {
        ELLE_ASSERT_NEQ(key, nullptr);
        ELLE_ASSERT_NEQ(key->pkey.dh->pub_key, nullptr);
        ELLE_ASSERT_NEQ(key->pkey.dh->priv_key, nullptr);

        // Make sure the cryptographic system is set up.
        cryptography::require();

        this->_check();
      }

      PrivateKey::PrivateKey(::DH* dh)
      {
        ELLE_ASSERT_NEQ(dh, nullptr);
        ELLE_ASSERT_NEQ(dh->pub_key, nullptr);
        ELLE_ASSERT_NEQ(dh->priv_key, nullptr);

        // Make sure the cryptographic system is set up.
        cryptography::require();

        if (::EVP_PKEY_type(this->_key->type) != EVP_PKEY_DH)
          throw Error(
            elle::sprintf("the EVP_PKEY key is not of type DH: %s",
                          ::EVP_PKEY_type(this->_key->type)));

        // Construct the private key based on the given DH structure.
        this->_construct(dh);

        this->_check();
      }

      PrivateKey::PrivateKey(PrivateKey const& other)
      {
        ELLE_ASSERT_NEQ(other._key->pkey.dh->pub_key, nullptr);
        ELLE_ASSERT_NEQ(other._key->pkey.dh->priv_key, nullptr);

        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Duplicate the DH structure.
        DH* _dh = low::DH_dup(other._key->pkey.dh);

        ELLE_CRYPTOGRAPHY_FINALLY_ACTION_FREE_DH(_dh);

        this->_construct(_dh);

        ELLE_CRYPTOGRAPHY_FINALLY_ABORT(_dh);

        this->_check();
      }

      PrivateKey::PrivateKey(PrivateKey&& other):
        _key(std::move(other._key))
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();

        this->_check();
      }

      /*--------.
      | Methods |
      `--------*/

      void
      PrivateKey::_construct(::DH* dh)
      {
        ELLE_ASSERT_NEQ(dh, nullptr);

        // Initialise the private key structure.
        ELLE_ASSERT_EQ(this->_key, nullptr);
        this->_key.reset(::EVP_PKEY_new());

        if (this->_key == nullptr)
          throw Error(
            elle::sprintf("unable to allocate the EVP_PKEY structure: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        // Set the dh structure into the private key.
        if (::EVP_PKEY_assign_DH(this->_key.get(), dh) <= 0)
          throw Error(
            elle::sprintf("unable to assign the DH key to the EVP_PKEY "
                          "structure: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));
      }

      void
      PrivateKey::_check() const
      {
        ELLE_ASSERT_NEQ(this->_key, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.dh, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.dh->pub_key, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.dh->priv_key, nullptr);
      }

      SecretKey
      PrivateKey::agree(PublicKey const& peer_K) const
      {
        return (raw::asymmetric::agree(this->_key.get(),
                                       peer_K.key().get()));
      }

      uint32_t
      PrivateKey::size() const
      {
        return (static_cast<uint32_t>(
                  ::EVP_PKEY_size(this->_key.get())));
      }

      uint32_t
      PrivateKey::length() const
      {
        return (static_cast<uint32_t>(
                  ::EVP_PKEY_bits(this->_key.get())));
      }

      /*----------.
      | Operators |
      `----------*/

      bool
      PrivateKey::operator ==(PrivateKey const& other) const
      {
        if (this == &other)
          return (true);

        ELLE_ASSERT_NEQ(this->_key, nullptr);
        ELLE_ASSERT_NEQ(other._key, nullptr);

        // Compare the public components because it is sufficient to
        // uniquely distinguish keys.
        return (::EVP_PKEY_cmp(this->_key.get(), other._key.get()) == 1);
      }

      /*----------.
      | Printable |
      `----------*/

      void
      PrivateKey::print(std::ostream& stream) const
      {
        ELLE_ASSERT_NEQ(this->_key, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.dh, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.dh->pub_key, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.dh->priv_key, nullptr);

        stream << "("
               << *this->_key->pkey.dh->pub_key
               << ", "
               << *this->_key->pkey.dh->priv_key
               << ")";
      }
    }
  }
}
