#include <cryptography/PublicKey.hh>
#include <cryptography/Digest.hh>
#include <cryptography/SecretKey.hh>
#include <cryptography/SecretKey.hh>
#include <cryptography/Code.hh>
#include <cryptography/Cipher.hh>
#include <cryptography/cryptography.hh>
#include <cryptography/bn.hh>

#include <elle/idiom/Open.hh>

namespace infinit
{
  namespace cryptography
  {
    /*----------.
    | Constants |
    `----------*/

    oneway::Algorithm const PublicKey::Constants::oneway_algorithm(
      oneway::Algorithm::sha256);

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// this method initializes the object.
    ///
    PublicKey::PublicKey():
      _key(nullptr),
      _contexts{nullptr, nullptr, nullptr}
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    ///
    /// this is the copy constructor.
    ///
    PublicKey::PublicKey(const PublicKey& K) :
      Object(K),
      _key(nullptr),
      _contexts{nullptr, nullptr, nullptr}
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();

      // re-create the public key by duplicate the internal numbers.
      if (K._key)
        if (this->Create(K._key) == elle::Status::Error)
          throw elle::Exception("unable to duplicate the public key");
    }

    ///
    /// this method releases the resources.
    ///
    PublicKey::~PublicKey()
    {
      // release the resources.
      if (this->_key != nullptr)
        ::EVP_PKEY_free(this->_key);

      if (this->_contexts.encrypt != nullptr)
        ::EVP_PKEY_CTX_free(this->_contexts.encrypt);

      if (this->_contexts.verify != nullptr)
        ::EVP_PKEY_CTX_free(this->_contexts.verify);

      if (this->_contexts.decrypt != nullptr)
        ::EVP_PKEY_CTX_free(this->_contexts.decrypt);
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method constructs a valid public key.
    ///
    elle::Status PublicKey::Create(::EVP_PKEY const* key)
    {
      assert(key != nullptr);
      assert(this->_key == nullptr);

      // call the creation method.
      if (this->Create(::BN_dup(key->pkey.rsa->n),
                       ::BN_dup(key->pkey.rsa->e)) == elle::Status::Error)
        escape("unable to create the public key");

      return elle::Status::Ok;
    }

    ///
    /// this method constructs a valid public key given the proper
    /// numbers.
    ///
    elle::Status PublicKey::Create(elle::Large* n, elle::Large* e)
    {
      assert(n != nullptr);
      assert(e != nullptr);

      // initialise the public key structure.
      if ((this->_key = ::EVP_PKEY_new()) == nullptr)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      ::RSA* rsa;
      // create the RSA structure.
      if ((rsa = ::RSA_new()) == nullptr)
        {
          ::RSA_free(rsa);
          escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));
        }

      // assign the big numbers relevant to the public key.
      rsa->n = n;
      rsa->e = e;

      // set the rsa structure into the public key.
      if (::EVP_PKEY_assign_RSA(this->_key, rsa) <= 0)
        {
          ::RSA_free(rsa);
          escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));
        }

      //
      // contexts
      //

      // create, initialize and configure---by setting the padding---the
      // encrypt context.
      if ((this->_contexts.encrypt =
           ::EVP_PKEY_CTX_new(this->_key, nullptr)) == nullptr)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      if (::EVP_PKEY_encrypt_init(this->_contexts.encrypt) <= 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      if (::EVP_PKEY_CTX_ctrl(this->_contexts.encrypt,
                              EVP_PKEY_RSA,
                              -1,
                              EVP_PKEY_CTRL_RSA_PADDING,
                              RSA_PKCS1_OAEP_PADDING,
                              nullptr) <= 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      // create and initialize the verify context.
      if ((this->_contexts.verify =
           ::EVP_PKEY_CTX_new(this->_key, nullptr)) == nullptr)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      if (::EVP_PKEY_verify_init(this->_contexts.verify) <= 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      if (::EVP_PKEY_CTX_ctrl(this->_contexts.verify,
                              EVP_PKEY_RSA,
                              -1,
                              EVP_PKEY_CTRL_RSA_PADDING,
                              RSA_PKCS1_PADDING,
                              nullptr) <= 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      // create and initialize the decrypt context.
      if ((this->_contexts.decrypt =
           ::EVP_PKEY_CTX_new(this->_key, nullptr)) == nullptr)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      if (::EVP_PKEY_verify_recover_init(this->_contexts.decrypt) <= 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      if (::EVP_PKEY_CTX_ctrl(this->_contexts.decrypt,
                              EVP_PKEY_RSA,
                              -1,
                              EVP_PKEY_CTRL_RSA_PADDING,
                              RSA_PKCS1_PADDING,
                              nullptr) <= 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      assert(this->_key != nullptr);

      return elle::Status::Ok;
    }

    elle::Status  PublicKey::Encrypt(elle::WeakBuffer const& buffer,
                               Code&                            code) const
    {
      SecretKey         secret;
      Code              key; // XXX[allocate later]
      Cipher            data;

      // (i)
      {
        if (secret.Generate() == elle::Status::Error)
          escape("unable to generate the secret key");
      }

      // (ii)
      {
        // cipher the plain text with the secret key.
        if (secret.Encrypt(buffer, data) == elle::Status::Error)
          escape("unable to cipher the plain text with the secret key");
      }

      // (iii)
      {
        // XXX secret_buf should be filled with zeros at the end
        elle::Buffer secret_buf;
        size_t size;

        // XXX
        secret_buf.writer() << secret;

        assert(secret_buf.size() > 0);

        assert(this->_contexts.encrypt != nullptr);

        // compute the size of the archived symmetric key.
        if (::EVP_PKEY_encrypt(
              this->_contexts.encrypt,
              nullptr,
              &size,
              secret_buf.contents(),
              secret_buf.size()) <= 0)
          escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

        // allocate memory so the key can receive the upcoming
        // encrypted portion.
        key.buffer().size(size);

        // actually encrypt the secret key's archive, storing the encrypted
        // portion directly into the key object, without any re-copy.
        if (::EVP_PKEY_encrypt(
              this->_contexts.encrypt,
              reinterpret_cast<unsigned char*>(key.buffer().mutable_contents()),
              &size,
              reinterpret_cast<const unsigned char*>(secret_buf.contents()),
              secret_buf.size()) <= 0)
          escape("key has size %lu, data has size %lu: %s", size, secret_buf.size(), ::ERR_error_string(ERR_get_error(), nullptr));

        // set the key size.
        key.buffer().size(size);
      }

      // (iv)
      {
        // XXX
        code.buffer().writer() << key << data;
      }

      return elle::Status::Ok;
    }

    elle::Status PublicKey::Verify(const Signature&                 signature,
                             elle::WeakBuffer const& buffer) const
    {
      // XXX[remove Plain(buffer) in favor of plain which should be the argument]
      // compute the plain's digest.
      Digest digest(oneway::hash(buffer,
                                 PublicKey::Constants::oneway_algorithm));

      // verify.
      int result =
        ::EVP_PKEY_verify(
          this->_contexts.verify,
          reinterpret_cast<const unsigned char*>(signature.region.contents),
          signature.region.size,
          reinterpret_cast<const unsigned char*>(digest.buffer().contents()),
          digest.buffer().size());

      switch (result)
        {
        case 1:
          return elle::Status::Ok;
        case 0:
          return elle::Status::Error;
        default:
          escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));
        }

      elle::unreachable();
    }

    elle::Status PublicKey::Decrypt(Code const& in, elle::Buffer& out) const
    {
      SecretKey         secret;

      Code              key;
      Cipher            data;

      // (i)
      in.buffer().reader() >> key >> data;

      // (ii)
      {
        size_t          size;

        // compute the size of the decrypted portion to come.
        if (::EVP_PKEY_verify_recover(
              this->_contexts.decrypt,
              nullptr,
              &size,
              reinterpret_cast<const unsigned char*>(key.buffer().contents()),
              key.buffer().size()) <= 0)
          escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

        elle::Buffer buffer(size);

        // perform the decrypt operation.
        //
        // note that since the encryption with the private key relies
        // upon the sign() EVP functionality, the verify_recover()
        // function is used here.
        if (::EVP_PKEY_verify_recover(
              this->_contexts.decrypt,
              reinterpret_cast<unsigned char*>(buffer.mutable_contents()),
              &size,
              reinterpret_cast<const unsigned char*>(key.buffer().contents()),
              key.buffer().size()) <= 0)
          escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

        // XXX
        buffer.size(size);

        // XXX
        buffer.reader() >> secret;
      }

      // (iii)
      {
        // finally, decrypt the data with the secret key.
        if (secret.Decrypt(data, out) == elle::Status::Error)
          escape("unable to decrypt the data with the secret key");
      }

      return elle::Status::Ok;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method checks if two objects match.
    ///
    elle::Boolean PublicKey::operator==(const PublicKey& element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return true;

      // if one of the key is null....
      if ((this->_key == nullptr) || (element._key == nullptr))
        {
          // compare the addresses.
          if (this->_key != element._key)
            return false;
        }
      else
        {
          // compare the internal numbers.
          if ((::BN_cmp(this->_key->pkey.rsa->n,
                        element._key->pkey.rsa->n) != 0) ||
              (::BN_cmp(this->_key->pkey.rsa->e,
                        element._key->pkey.rsa->e) != 0))
            return false;
        }

      return true;
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(PublicKey, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the public key internals.
    ///
    elle::Status              PublicKey::Dump(const elle::Natural32         margin) const
    {
      elle::String            alignment(margin, ' ');

      // display depending on the value.
      if (this->_key == nullptr)
        {
          std::cout << alignment << "[PublicKey] " << elle::none << std::endl;
        }
      else
        {
          std::cout << alignment << "[PublicKey] " << std::endl;

          // dump the internal numbers.
          std::cout << alignment << elle::io::Dumpable::Shift << "[n] "
                    << *this->_key->pkey.rsa->n << std::endl;

          std::cout << alignment << elle::io::Dumpable::Shift << "[e] "
                    << *this->_key->pkey.rsa->e << std::endl;
        }

      return elle::Status::Ok;
    }

//
// ---------- printable -------------------------------------------------------
//

    void
    PublicKey::print(std::ostream& stream) const
    {
      stream << "public key(XXX)";
    }

  }
}
