#include <openssl/engine.h>
#include <openssl/crypto.h>
#include <openssl/dh.h>
#include <openssl/err.h>
#include <openssl/pem.h>

#include <elle/Error.hh>
#include <elle/finally.hh>
#include <elle/log.hh>
#include <elle/printf.hh>

#include <cryptography/dh/PublicKey.hh>
#include <cryptography/dh/PrivateKey.hh>
#include <cryptography/dh/KeyPair.hh>
#include <cryptography/dh/low.hh>
#include <cryptography/context.hh>
#include <cryptography/Error.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/bn.hh>
#include <cryptography/finally.hh>
#include <cryptography/hash.hh>

//
// ---------- Class -----------------------------------------------------------
//

namespace infinit
{
  namespace cryptography
  {
    namespace dh
    {
      /*-------------.
      | Construction |
      `-------------*/

      PublicKey::PublicKey(PrivateKey const& k)
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Extract the public key only.
        DH* _dh = low::DH_priv2pub(k.key().get()->pkey.dh);

        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_DH(_dh);

        // Construct the public key based on the given DH structure whose
        // ownership is retained.
        this->_construct(_dh);

        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(_dh);

        this->_check();
      }

      PublicKey::PublicKey(::EVP_PKEY* key):
        _key(key)
      {
        ELLE_ASSERT_NEQ(key, nullptr);
        ELLE_ASSERT_NEQ(key->pkey.dh->pub_key, nullptr);
        ELLE_ASSERT_EQ(key->pkey.dh->priv_key, nullptr);

        // Make sure the cryptographic system is set up.
        cryptography::require();

        if (::EVP_PKEY_type(this->_key->type) != EVP_PKEY_DH)
          throw Error(
            elle::sprintf("the EVP_PKEY key is not of type DH: %s",
                          ::EVP_PKEY_type(this->_key->type)));

        this->_check();
      }

      PublicKey::PublicKey(::DH* dh)
      {
        ELLE_ASSERT_NEQ(dh, nullptr);
        ELLE_ASSERT_NEQ(dh->pub_key, nullptr);
        ELLE_ASSERT_EQ(dh->priv_key, nullptr);

        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Construct the public key based on the given DH structure.
        this->_construct(dh);

        this->_check();
      }

      PublicKey::PublicKey(PublicKey const& other)
      {
        ELLE_ASSERT_NEQ(other._key->pkey.dh->pub_key, nullptr);

        // Make sure the cryptographic system is set up.
        cryptography::require();

        // Duplicate the DH structure.
        DH* _dh = low::DH_dup(other._key->pkey.dh);

        INFINIT_CRYPTOGRAPHY_FINALLY_ACTION_FREE_DH(_dh);

        this->_construct(_dh);

        INFINIT_CRYPTOGRAPHY_FINALLY_ABORT(_dh);

        this->_check();
      }

      PublicKey::PublicKey(PublicKey&& other):
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
      PublicKey::_construct(::DH* dh)
      {
        ELLE_ASSERT_NEQ(dh, nullptr);

        // Initialise the public key structure.
        ELLE_ASSERT_EQ(this->_key, nullptr);
        this->_key.reset(::EVP_PKEY_new());

        if (this->_key == nullptr)
          throw Error(
            elle::sprintf("unable to allocate the EVP_PKEY structure: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));

        // Set the dh structure into the public key.
        if (::EVP_PKEY_assign_DH(this->_key.get(), dh) <= 0)
          throw Error(
            elle::sprintf("unable to assign the DH key to the EVP_PKEY "
                          "structure: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));
      }

      void
      PublicKey::_check() const
      {
        ELLE_ASSERT_NEQ(this->_key, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.dh, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.dh->pub_key, nullptr);
        ELLE_ASSERT_EQ(this->_key->pkey.dh->priv_key, nullptr);
      }

      uint32_t
      PublicKey::size() const
      {
        return (static_cast<uint32_t>(
                  ::EVP_PKEY_size(this->_key.get())));
      }

      uint32_t
      PublicKey::length() const
      {
        return (static_cast<uint32_t>(
                  ::EVP_PKEY_bits(this->_key.get())));
      }

      /*----------.
      | Operators |
      `----------*/

      bool
      PublicKey::operator ==(PublicKey const& other) const
      {
        if (this == &other)
          return (true);

        ELLE_ASSERT_NEQ(this->_key, nullptr);
        ELLE_ASSERT_NEQ(other._key, nullptr);

        return (::EVP_PKEY_cmp(this->_key.get(), other._key.get()) == 1);
      }

      /*----------.
      | Printable |
      `----------*/

      void
      PublicKey::print(std::ostream& stream) const
      {
        ELLE_ASSERT_NEQ(this->_key, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.dh, nullptr);
        ELLE_ASSERT_NEQ(this->_key->pkey.dh->pub_key, nullptr);

        stream << "("
               << *this->_key->pkey.dh->pub_key
               << ")";
      }
    }
  }
}
