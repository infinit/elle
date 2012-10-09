#include <elle/cryptography/PublicKey.hh>
#include <elle/cryptography/Digest.hh>
#include <elle/cryptography/OneWay.hh>
#include <elle/cryptography/SecretKey.hh>
#include <elle/cryptography/SecretKey.hh>
#include <elle/cryptography/Code.hh>
#include <elle/cryptography/Cipher.hh>

#include <elle/standalone/Log.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  namespace cryptography
  {

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
    }

    ///
    /// this is the copy constructor.
    ///
    PublicKey::PublicKey(const PublicKey& K) :
      Object(K),
      _key(nullptr),
      _contexts{nullptr, nullptr, nullptr}
    {
      // re-create the public key by duplicate the internal numbers.
      if (this->Create(K._key) == Status::Error)
        fail("unable to duplicate the public key");
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
    Status PublicKey::Create(::EVP_PKEY const* key)
    {
      assert(key != nullptr);
      assert(this->_key == nullptr);

      // call the creation method.
      if (this->Create(::BN_dup(key->pkey.rsa->n),
                       ::BN_dup(key->pkey.rsa->e)) == Status::Error)
        escape("unable to create the public key");

      return Status::Ok;
    }

    ///
    /// this method constructs a valid public key given the proper
    /// numbers.
    ///
    Status PublicKey::Create(Large* n, Large* e)
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
      this->_contexts.encrypt = ::EVP_PKEY_CTX_new(this->_key, nullptr);
      if (this->_contexts.encrypt == nullptr)
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
      this->_contexts.verify = EVP_PKEY_CTX_new(this->_key, nullptr);
      if (this->_contexts.verify == nullptr)
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
      this->_contexts.decrypt = ::EVP_PKEY_CTX_new(this->_key, nullptr);
      if (this->_contexts.decrypt == nullptr)
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

      return Status::Ok;
    }

    Status  PublicKey::Encrypt(elle::utility::WeakBuffer const& buffer,
                               Code&                            code) const
    {
      SecretKey         secret;

      Code              key;
      Cipher            data;

      // (i)
      {
        if (secret.Generate() == Status::Error)
          escape("unable to generate the secret key");
      }

      // (ii)
      {
        // cipher the plain text with the secret key.
        if (secret.Encrypt(buffer, data) == Status::Error)
          escape("unable to cipher the plain text with the secret key");
      }

      // (iii)
      {
        // XXX secret_buf should be filled with zeros at the end
        elle::utility::Buffer secret_buf;
        size_t                size;

        try
          {
            secret_buf.Writer() << secret;
            assert(secret_buf.Size() > 0);
          }
        catch (std::runtime_error const& err)
          {
            escape("Cannot serialize the secret key: %s", err.what());
          }

        assert(this->_contexts.encrypt != nullptr);

        // compute the size of the archived symmetric key.
        if (::EVP_PKEY_encrypt(
              this->_contexts.encrypt,
              nullptr,
              &size,
              secret_buf.Contents(),
              secret_buf.Size()) <= 0)
          escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

        // allocate memory so the key can receive the upcoming
        // encrypted portion.
        if (key.region.Prepare(size) == Status::Error)
          escape("unable to prepare the key");

        // actually encrypt the secret key's archive, storing the encrypted
        // portion directly into the key object, without any re-copy.
        if (::EVP_PKEY_encrypt(
              this->_contexts.encrypt,
              reinterpret_cast<unsigned char*>(key.region.contents),
              &size,
              reinterpret_cast<const unsigned char*>(secret_buf.Contents()),
              secret_buf.Size()) <= 0)
          escape("key has size %lu, data has size %lu: %s", size, secret_buf.size(), ::ERR_error_string(ERR_get_error(), nullptr));

        // set the key size.
        key.region.size = size;
      }

      // (iv)
      {
        elle::utility::Buffer result_buf;

        try
          {
            result_buf.Writer() << key << data;
          }
        catch (std::exception const& err)
          {
            escape(
              "unable to serialize the asymetrically-encrypted secret key "
              "and the symetrically-encrypted data: %s",
              err.what()
                   );
          }

        // XXX this copy is not needed
        // duplicate the archive's content.
        if (code.region.Duplicate(result_buf.Contents(),
                                  result_buf.Size()) == Status::Error)
          escape("unable to duplicate the archive's content");
      }

      return Status::Ok;
    }

    Status PublicKey::Verify(const Signature&                 signature,
                             elle::utility::WeakBuffer const& buffer) const
    {
      Digest            digest;

      // compute the plain's digest.
      if (OneWay::Hash(buffer, digest) == Status::Error)
        escape("unable to hash the plain");

      // verify.
      if (::EVP_PKEY_verify(
            this->_contexts.verify,
            reinterpret_cast<const unsigned char*>(signature.region.contents),
            signature.region.size,
            reinterpret_cast<const unsigned char*>(digest.region.contents),
            digest.region.size) <= 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      return Status::Ok;
    }

    Status PublicKey::Decrypt(Code const& in, elle::utility::Buffer& out) const
    {
      SecretKey         secret;

      Code              key;
      Cipher            data;

      // (i)
      try
        {
          elle::utility::WeakBuffer input_buffer(in.region.contents, in.region.size);
          input_buffer.Reader() >> key >> data;
        }
      catch (std::exception const& err)
        {
          escape(
            "unable to extract the asymetrically-encrypted secret key "
            "and the symetrically-encrypted data: %s",
            err.what()
                 );
        }

      // (ii)
      {
        size_t          size;

        // compute the size of the decrypted portion to come.
        if (::EVP_PKEY_verify_recover(
              this->_contexts.decrypt,
              nullptr,
              &size,
              reinterpret_cast<const unsigned char*>(key.region.contents),
              key.region.size) <= 0)
          escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

        elle::utility::Buffer buf;

        try
          {
            buf.Size(size);
          }
        catch (std::exception const& err)
          {
            escape("unable to allocate the required memory: %s", err.what());
          }

        auto buf_pair = buf.Release();

        // perform the decrypt operation.
        //
        // note that since the encryption with the private key relies
        // upon the sign() EVP functionality, the verify_recover()
        // function is used here.
        if (::EVP_PKEY_verify_recover(
              this->_contexts.decrypt,
              reinterpret_cast<unsigned char*>(buf_pair.first.get()),
              &size,
              reinterpret_cast<const unsigned char*>(key.region.contents),
              key.region.size) <= 0)
          escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

        buf_pair.second = size;


        try
          {
            elle::utility::Buffer(std::move(buf_pair)).Reader() >> secret;
          }
        catch (std::exception const& err)
          {
            escape("couldn't decode the object: %s", err.what());
          }
      }

      // (iii)
      {
        // finally, decrypt the data with the secret key.
        if (secret.Decrypt(data, out) == Status::Error)
          escape("unable to decrypt the data with the secret key");
      }

      return Status::Ok;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method checks if two objects match.
    ///
    Boolean PublicKey::operator==(const PublicKey& element) const
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
    Status              PublicKey::Dump(const Natural32         margin) const
    {
      String            alignment(margin, ' ');

      // display depending on the value.
      if (this->_key == nullptr)
        {
          std::cout << alignment << "[PublicKey] " << none << std::endl;
        }
      else
        {
          std::cout << alignment << "[PublicKey] " << std::endl;

          // dump the internal numbers.
          std::cout << alignment << io::Dumpable::Shift << "[n] "
                    << *this->_key->pkey.rsa->n << std::endl;

          std::cout << alignment << io::Dumpable::Shift << "[e] "
                    << *this->_key->pkey.rsa->e << std::endl;
        }

      return Status::Ok;
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
