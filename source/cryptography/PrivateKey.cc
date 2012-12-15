#include <cryptography/PrivateKey.hh>
#include <cryptography/PublicKey.hh>
#include <cryptography/SecretKey.hh>
#include <cryptography/Seed.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/SecretKey.hh>
#include <cryptography/Code.hh>
#include <cryptography/Cipher.hh>
#include <cryptography/finally.hh>
#include <cryptography/bn.hh>

#include <elle/Exception.hh>

#include <comet/Comet.hh>

#include <openssl/rsa.h>
#include <openssl/engine.h>
#include <openssl/crypto.h>
#include <openssl/err.h>

#include <elle/idiom/Open.hh> // XXX

namespace infinit
{
  namespace cryptography
  {
    /*----------.
    | Constants |
    `----------*/

    oneway::Algorithm const PrivateKey::Constants::oneway_algorithm(
      oneway::Algorithm::sha256);

    /*-------------.
    | Construction |
    `-------------*/

    PrivateKey::PrivateKey():
      _key(nullptr),
      _contexts{nullptr, nullptr, nullptr}
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    PrivateKey::PrivateKey(::EVP_PKEY const* key):
      PrivateKey(::BN_dup(key->pkey.rsa->n),
                 ::BN_dup(key->pkey.rsa->e),
                 ::BN_dup(key->pkey.rsa->d),
                 ::BN_dup(key->pkey.rsa->p),
                 ::BN_dup(key->pkey.rsa->q),
                 ::BN_dup(key->pkey.rsa->dmp1),
                 ::BN_dup(key->pkey.rsa->dmq1),
                 ::BN_dup(key->pkey.rsa->iqmp))
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    PrivateKey::PrivateKey(::BIGNUM* n,
                           ::BIGNUM* e,
                           ::BIGNUM* d,
                           ::BIGNUM* p,
                           ::BIGNUM* q,
                           ::BIGNUM* dmp1,
                           ::BIGNUM* dmq1,
                           ::BIGNUM* iqmp):
      _key(nullptr),
      _contexts{nullptr, nullptr, nullptr}
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();

      // Call a private method for constructing the object so as
      // to also be callable from the serialization mechanism, especially
      // the deserialization.
      this->_construct(n, e, d, p, q, dmp1, dmq1, iqmp);

      ELLE_ASSERT(this->_key != nullptr);
    }

    PrivateKey::PrivateKey(PrivateKey const& other):
      PrivateKey(other._key)
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    PrivateKey::~PrivateKey()
    {
      if (this->_key != nullptr)
        ::EVP_PKEY_free(this->_key);

      if (this->_contexts.decrypt != nullptr)
        ::EVP_PKEY_CTX_free(this->_contexts.decrypt);

      if (this->_contexts.sign != nullptr)
        ::EVP_PKEY_CTX_free(this->_contexts.sign);

      if (this->_contexts.encrypt != nullptr)
        ::EVP_PKEY_CTX_free(this->_contexts.encrypt);
    }

    /*--------.
    | Methods |
    `--------*/

    void
    PrivateKey::_construct(::BIGNUM* n,
                           ::BIGNUM* e,
                           ::BIGNUM* d,
                           ::BIGNUM* p,
                           ::BIGNUM* q,
                           ::BIGNUM* dmp1,
                           ::BIGNUM* dmq1,
                           ::BIGNUM* iqmp)
    {
      CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(n);
      CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(e);
      CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(d);
      CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(p);
      CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(q);
      CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(dmp1);
      CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(dmq1);
      CRYPTOGRAPHY_FINALLY_ACTION_FREE_BN(iqmp);

      // First, create the RSA key based on the given big numbers.

      // Initialise the private key structure.
      if ((this->_key = ::EVP_PKEY_new()) == nullptr)
        throw elle::Exception("%s",
                              ::ERR_error_string(ERR_get_error(), nullptr));

      ::RSA* rsa;

      // Create the RSA structure.
      if ((rsa = ::RSA_new()) == nullptr)
        throw elle::Exception("%s",
                              ::ERR_error_string(ERR_get_error(), nullptr));

      CRYPTOGRAPHY_FINALLY_ACTION_FREE_RSA(rsa);

      // Assign the big numbers relevant to the private key.
      rsa->n = n;
      rsa->e = e;
      rsa->d = d;
      rsa->p = p;
      rsa->q = q;
      rsa->dmp1 = dmp1;
      rsa->dmq1 = dmq1;
      rsa->iqmp = iqmp;

      CRYPTOGRAPHY_FINALLY_ABORT(n);
      CRYPTOGRAPHY_FINALLY_ABORT(e);
      CRYPTOGRAPHY_FINALLY_ABORT(d);
      CRYPTOGRAPHY_FINALLY_ABORT(p);
      CRYPTOGRAPHY_FINALLY_ABORT(q);
      CRYPTOGRAPHY_FINALLY_ABORT(dmp1);
      CRYPTOGRAPHY_FINALLY_ABORT(dmq1);
      CRYPTOGRAPHY_FINALLY_ABORT(iqmp);

      // Set the rsa structure into the private key.
      if (::EVP_PKEY_assign_RSA(this->_key, rsa) <= 0)
        throw elle::Exception("%s",
                              ::ERR_error_string(ERR_get_error(), nullptr));

      CRYPTOGRAPHY_FINALLY_ABORT(rsa);

      // Second, initialize the contexts associated with the private key.

      // Prepare the decrypt context.
      if ((this->_contexts.decrypt =
           ::EVP_PKEY_CTX_new(this->_key, nullptr)) == nullptr)
        throw elle::Exception("%s",
                              ::ERR_error_string(ERR_get_error(), nullptr));

      if (::EVP_PKEY_decrypt_init(this->_contexts.decrypt) <= 0)
        throw elle::Exception("%s",
                              ::ERR_error_string(ERR_get_error(), nullptr));

      if (::EVP_PKEY_CTX_ctrl(this->_contexts.decrypt,
                              EVP_PKEY_RSA,
                              -1,
                              EVP_PKEY_CTRL_RSA_PADDING,
                              RSA_PKCS1_OAEP_PADDING,
                              nullptr) <= 0)
        throw elle::Exception("%s",
                              ::ERR_error_string(ERR_get_error(), nullptr));

      // Prepare the sign context.
      if ((this->_contexts.sign =
           ::EVP_PKEY_CTX_new(this->_key, nullptr)) == nullptr)
        throw elle::Exception("%s",
                              ::ERR_error_string(ERR_get_error(), nullptr));

      if (::EVP_PKEY_sign_init(this->_contexts.sign) <= 0)
        throw elle::Exception("%s",
                              ::ERR_error_string(ERR_get_error(), nullptr));

      if (::EVP_PKEY_CTX_ctrl(this->_contexts.sign,
                              EVP_PKEY_RSA,
                              -1,
                              EVP_PKEY_CTRL_RSA_PADDING,
                              RSA_PKCS1_PADDING,
                              nullptr) <= 0)
        throw elle::Exception("%s",
                              ::ERR_error_string(ERR_get_error(), nullptr));

      // Prepare the encrypt context.
      if ((this->_contexts.encrypt =
           ::EVP_PKEY_CTX_new(this->_key, nullptr)) == nullptr)
        throw elle::Exception("%s",
                              ::ERR_error_string(ERR_get_error(), nullptr));

      if (::EVP_PKEY_sign_init(this->_contexts.encrypt) <= 0)
        throw elle::Exception("%s",
                              ::ERR_error_string(ERR_get_error(), nullptr));

      if (::EVP_PKEY_CTX_ctrl(this->_contexts.encrypt,
                              EVP_PKEY_RSA,
                              -1,
                              EVP_PKEY_CTRL_RSA_PADDING,
                              RSA_PKCS1_PADDING,
                              nullptr) <= 0)
        throw elle::Exception("%s",
                              ::ERR_error_string(ERR_get_error(), nullptr));
    }

    Clear
    PrivateKey::decrypt(Code const& code) const
    {
      // 1) Extract the key and ciphered data from the code which
      //    is supposed to be an archive.
      Code key;
      Cipher data;

      code.buffer().reader() >> key >> data;

      // 2) decrypt the key so as to reveal the symmetric secret key.
      size_t size;

      // Compute the size of the decrypted portion to come.
      if (::EVP_PKEY_decrypt(
            this->_contexts.decrypt,
            nullptr,
            &size,
            key.buffer().contents(),
            key.buffer().size()) <= 0)
        throw elle::Exception("%s",
                              ::ERR_error_string(ERR_get_error(), nullptr));

      elle::Buffer buffer(size);

      // Perform the decrypt operation.
      if (::EVP_PKEY_decrypt(
            this->_contexts.decrypt,
            buffer.mutable_contents(),
            &size,
            key.buffer().contents(),
            key.buffer().size()) <= 0)
        throw elle::Exception("%s",
                              ::ERR_error_string(ERR_get_error(), nullptr));

      // Set the final buffer size.
      buffer.size(size);

      // Finally extract the secret key since decrypted.
      SecretKey secret;
      buffer.reader() >> secret;

      // 3) Decrypt the data with the secret key.
      Clear clear{secret.decrypt(data)};

      return (clear);
    }

    Signature
    PrivateKey::sign(elle::WeakBuffer const& buffer) const
    {
      Signature signature;
      ::size_t size;

      // XXX[remove Plain(buffer) in favor of plain which should be the argument]
      // Compute the plain's digest.
      Digest digest(oneway::hash(buffer,
                                 PrivateKey::Constants::oneway_algorithm));

      // Retrieve information on the size of the output signature.
      if (::EVP_PKEY_sign(
            this->_contexts.sign,
            nullptr,
            &size,
            reinterpret_cast<const unsigned char*>(digest.buffer().contents()),
            digest.buffer().size()) <= 0)
        throw elle::Exception("%s",
                              ::ERR_error_string(ERR_get_error(), nullptr));

      // Prepare the signature so it can receive the upcoming portion.
      if (signature.region.Prepare(size) == elle::Status::Error)
        throw elle::Exception("unable to prepare the signature");

      // actually sign the portion.
      if (::EVP_PKEY_sign(
            this->_contexts.sign,
            reinterpret_cast<unsigned char*>(signature.region.contents),
            &size,
            reinterpret_cast<const unsigned char*>(digest.buffer().contents()),
            digest.buffer().size()) <= 0)
        throw elle::Exception("%s",
                              ::ERR_error_string(ERR_get_error(), nullptr));

      // set the code size.
      signature.region.size = size;

      return (signature);
    }

    elle::Status PrivateKey::Encrypt(elle::WeakBuffer const& in,
                               Code&                            out) const
    {
      Code key; // XXX[allocate later]

      // (i) Generate a temporary secret key.
      SecretKey secret{SecretKey::generate(512)}; // XXX[should not be static like that

      // (ii) Cipher the plain text with the temporary secret key.
      Cipher data{secret.encrypt(Plain{in})}; // XXX[remove plain]

      // (iii)
      {
        size_t size;
        elle::Buffer buffer;

        // XXX
        buffer.writer() << secret;

        // compute the size of the archived symmetric key.
        if (::EVP_PKEY_sign(
              this->_contexts.encrypt,
              nullptr,
              &size,
              buffer.contents(),
              buffer.size()) <= 0)
          escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

        // allocate memory so the key can receive the upcoming
        // encrypted portion.
        key.buffer().size(size);

        // actually encrypt the secret key's archive, storing the encrypted
        // portion directly into the key object, without any re-copy.
        //
        // note that since the encryption is performed with the private key,
        // the operation is equivalent to a signature.
        if (::EVP_PKEY_sign(
              this->_contexts.encrypt,
              key.buffer().mutable_contents(),
              &size,
              buffer.contents(),
              buffer.size()) <= 0)
          escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

        // set the key size.
        key.buffer().size(size);
      }

      // (iv)
      {
        // XXX
        out.buffer().writer() << key << data;
      }

      return elle::Status::Ok;
    }

    ///
    /// this method derives a public key according to (i) its complementary
    /// private key and (ii) the seed used for rotating this key pair.
    ///
    elle::Status              PrivateKey::Derive(const Seed&          seed,
                                           PublicKey&           K) const
    {
      struct Scope
      {
        ::EVP_PKEY*       key;
        ::RSA*            rsa;

        Scope() : key(nullptr), rsa(nullptr) {}
        ~Scope() { ::EVP_PKEY_free(this->key); ::RSA_free(this->rsa); }
      } scope;

      // create an EVP key.
      if ((scope.key = ::EVP_PKEY_new()) == nullptr)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      // create a new RSA key.
      if ((scope.rsa = ::RSA_new()) == nullptr)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      // derive the RSA key.
      if (comet::RSA_derive(scope.rsa,
                            this->_key->pkey.rsa->n,
                            seed.region.contents,
                            seed.region.size) <= 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      // assign the RSA key to the EVP's.
      if (::EVP_PKEY_assign_RSA(scope.key, scope.rsa) <= 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      // stop tracking.
      scope.rsa = nullptr;

      // create the rotated public key according to the EVP structure.
      if (K.Create(scope.key) == elle::Status::Error)
        escape("unable to create the public key");

      return elle::Status::Ok;
    }

    /*----------.
    | Operators |
    `----------*/

    elle::Boolean
    PrivateKey::operator ==(PrivateKey const& other) const
    {
      if (this == &other)
        return (true);

      ELLE_ASSERT(this->_key != nullptr);
      ELLE_ASSERT(other._key != nullptr);

      // Compare the internal numbers.
      return ((::BN_cmp(this->_key->pkey.rsa->n,
                        other._key->pkey.rsa->n) == 0) &&
              (::BN_cmp(this->_key->pkey.rsa->d,
                        other._key->pkey.rsa->d) == 0) &&
              (::BN_cmp(this->_key->pkey.rsa->p,
                        other._key->pkey.rsa->p) == 0) &&
              (::BN_cmp(this->_key->pkey.rsa->q,
                        other._key->pkey.rsa->q) == 0));
    }

    /*---------.
    | Dumpable |
    `---------*/

    elle::Status
    PrivateKey::Dump(const elle::Natural32        margin) const
    {
      elle::String            alignment(margin, ' ');

      std::cout << alignment << "[PrivateKey]" << std::endl;

      ELLE_ASSERT(this->_key != nullptr);

      // dump the internal numbers.
      std::cout << alignment << elle::io::Dumpable::Shift << "[n] "
                << *this->_key->pkey.rsa->n << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift << "[d] "
                << *this->_key->pkey.rsa->d << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift << "[p] "
                << *this->_key->pkey.rsa->p << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift << "[q] "
                << *this->_key->pkey.rsa->q << std::endl;

      return elle::Status::Ok;
    }

    /*----------.
    | Printable |
    `----------*/

    void
    PrivateKey::print(std::ostream& stream) const
    {
      stream << "k{"
             << *this->_key->pkey.rsa->n
             << ", "
             << *this->_key->pkey.rsa->d
             << "}";
    }
  }
}
