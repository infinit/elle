#include <elle/cryptography/PrivateKey.hh>
#include <elle/cryptography/PublicKey.hh>
#include <elle/cryptography/OneWay.hh>
#include <elle/cryptography/SecretKey.hh>
#include <elle/cryptography/Seed.hh>
#include <elle/Exception.hh>

#include <elle/cryptography/SecretKey.hh>
#include <elle/cryptography/Code.hh>
#include <elle/cryptography/Cipher.hh>
#include <elle/standalone/Region.hh>

#include <comet/Comet.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  namespace cryptography
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this defines a null private key.
    ///
    const PrivateKey            PrivateKey::Null;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// this method initializes the object.
    ///
    PrivateKey::PrivateKey():
      _key(nullptr),
      _contexts{nullptr, nullptr, nullptr}
    {}

    ///
    /// this is the copy constructor.
    ///
    PrivateKey::PrivateKey(PrivateKey const&k) :
      Object(k),
      _key(nullptr),
      _contexts{nullptr, nullptr, nullptr}
    {
      assert(k._key != nullptr);

      // create the private key by duplicating the internal numbers.
      if (this->Create(k._key) == Status::Error)
        throw Exception("unable to duplicate the private key");

      assert(this->_key != nullptr);
    }

    ///
    /// this method releases the resources.
    ///
    PrivateKey::~PrivateKey()
    {
      // release the resources.
      if (this->_key != nullptr)
        ::EVP_PKEY_free(this->_key);

      if (this->_contexts.decrypt != nullptr)
        ::EVP_PKEY_CTX_free(this->_contexts.decrypt);

      if (this->_contexts.sign != nullptr)
        ::EVP_PKEY_CTX_free(this->_contexts.sign);

      if (this->_contexts.encrypt != nullptr)
        ::EVP_PKEY_CTX_free(this->_contexts.encrypt);

      this->_key = nullptr;
      this->_contexts.decrypt = nullptr;
      this->_contexts.sign = nullptr;
      this->_contexts.encrypt = nullptr;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method constructs a valid private key.
    ///
    Status              PrivateKey::Create(const ::EVP_PKEY*    key)
    {
      assert(this->_key == nullptr);

      if (this->Create(::BN_dup(key->pkey.rsa->n),
                       ::BN_dup(key->pkey.rsa->e),
                       ::BN_dup(key->pkey.rsa->d),
                       ::BN_dup(key->pkey.rsa->p),
                       ::BN_dup(key->pkey.rsa->q),
                       ::BN_dup(key->pkey.rsa->dmp1),
                       ::BN_dup(key->pkey.rsa->dmq1),
                       ::BN_dup(key->pkey.rsa->iqmp)) == Status::Error)
        escape("unable to create the private key");

      assert(this->_key != nullptr);

      return Status::Ok;
    }

    ///
    /// this method constructs a valid private key given the proper
    /// numbers.
    ///
    Status              PrivateKey::Create(Large*               n,
                                           Large*               e,
                                           Large*               d,
                                           Large*               p,
                                           Large*               q,
                                           Large*               dmp1,
                                           Large*               dmq1,
                                           Large*               iqmp)
    {
      ::RSA*            rsa;

      //
      // key
      //

      assert(this->_key == nullptr);

      // initialise the private key structure.
      if ((this->_key = ::EVP_PKEY_new()) == nullptr)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      // create the RSA structure.
      if ((rsa = ::RSA_new()) == nullptr)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));


      // assign the big numbers relevant to the private key.
      rsa->n = n;
      rsa->e = e;
      rsa->d = d;
      rsa->p = p;
      rsa->q = q;
      rsa->dmp1 = dmp1;
      rsa->dmq1 = dmq1;
      rsa->iqmp = iqmp;

      // set the rsa structure into the private key.
      if (::EVP_PKEY_assign_RSA(this->_key, rsa) <= 0)
        {
          ::RSA_free(rsa);
          escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));
        }

      //
      // contexts
      //

      // create, initialize and configure the decrypt context.
      if ((this->_contexts.decrypt = ::EVP_PKEY_CTX_new(this->_key, nullptr)) == nullptr)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      if (::EVP_PKEY_decrypt_init(this->_contexts.decrypt) <= 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      if (::EVP_PKEY_CTX_ctrl(this->_contexts.decrypt,
                              EVP_PKEY_RSA,
                              -1,
                              EVP_PKEY_CTRL_RSA_PADDING,
                              RSA_PKCS1_OAEP_PADDING,
                              nullptr) <= 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      // create and initialize a sign context.
      if ((this->_contexts.sign =
           ::EVP_PKEY_CTX_new(this->_key, nullptr)) == nullptr)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      if (::EVP_PKEY_sign_init(this->_contexts.sign) <= 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      if (::EVP_PKEY_CTX_ctrl(this->_contexts.sign,
                              EVP_PKEY_RSA,
                              -1,
                              EVP_PKEY_CTRL_RSA_PADDING,
                              RSA_PKCS1_PADDING,
                              nullptr) <= 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      // create and initialize a encrypt context.
      if ((this->_contexts.encrypt =
           ::EVP_PKEY_CTX_new(this->_key, nullptr)) == nullptr)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      if (::EVP_PKEY_sign_init(this->_contexts.encrypt) <= 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      if (::EVP_PKEY_CTX_ctrl(this->_contexts.encrypt,
                              EVP_PKEY_RSA,
                              -1,
                              EVP_PKEY_CTRL_RSA_PADDING,
                              RSA_PKCS1_PADDING,
                              nullptr) <= 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      assert(this->_key != nullptr);

      return Status::Ok;
    }

    Status PrivateKey::Decrypt(elle::cryptography::Code const&    in,
                               elle::Buffer&                      out) const
    {
      SecretKey         secret;
      Code              key;
      Cipher            data;

      // (i)
      try
        {
          elle::WeakBuffer(
            in.region.contents,
            in.region.size
          ).reader() >> key >> data;
        }
      catch (std::runtime_error const& err)
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
        if (::EVP_PKEY_decrypt(
              this->_contexts.decrypt,
              nullptr,
              &size,
              key.region.contents,
              key.region.size) <= 0)
          escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

        elle::Buffer buf(size);

        // perform the decrypt operation.
        if (::EVP_PKEY_decrypt(
              this->_contexts.decrypt,
              buf.mutable_contents(),
              &size,
              key.region.contents,
              key.region.size) <= 0)
          escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

        try
          {
            buf.reader() >> secret;
          }
        catch (std::exception const& err)
          {
            escape(
              "unable to extract the secret key from the archive: %s",
              err.what()
            );
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

    Signature
    PrivateKey::sign(elle::WeakBuffer const&  plain) const
    {
      Signature signature;
      Digest digest;
      ::size_t size;

      // Compute the plain's digest.
      if (OneWay::Hash(plain, digest) == Status::Error)
        throw Exception("unable to hash the plain");

      // Retrieve information on the size of the output signature.
      if (::EVP_PKEY_sign(
            this->_contexts.sign,
            nullptr,
            &size,
            reinterpret_cast<const unsigned char*>(digest.region.contents),
            digest.region.size) <= 0)
        throw Exception("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      // Prepare the signature so it can receive the upcoming portion.
      if (signature.region.Prepare(size) == Status::Error)
        throw Exception("unable to prepare the signature");

      // actually sign the portion.
      if (::EVP_PKEY_sign(
            this->_contexts.sign,
            reinterpret_cast<unsigned char*>(signature.region.contents),
            &size,
            reinterpret_cast<const unsigned char*>(digest.region.contents),
            digest.region.size) <= 0)
        throw Exception("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      // set the code size.
      signature.region.size = size;

      return (signature);
    }

    Status PrivateKey::Encrypt(elle::WeakBuffer const& in,
                               Code&                            out) const
    {
      SecretKey         secret;
      Code              key;
      Cipher            data;

      // (i)
      {
        // generate a secret key.
        if (secret.Generate() == Status::Error)
          escape("unable to generate the secret key");
      }

      // (ii)
      {
        // cipher the plain text with the secret key.
        if (secret.Encrypt(in, data) == Status::Error)
          escape("unable to cipher the plain text with the secret key");
      }

      // (iii)
      {
        size_t                  size;
        elle::Buffer   buf;

        try
          {
            buf.writer() << secret;
          }
        catch (std::exception const& err)
          {
            escape("Cannot serialize secret key: %s", err.what());
          }

        // compute the size of the archived symmetric key.
        if (::EVP_PKEY_sign(
              this->_contexts.encrypt,
              nullptr,
              &size,
              buf.contents(),
              buf.size()) <= 0)
          escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

        // allocate memory so the key can receive the upcoming
        // encrypted portion.
        if (key.region.Prepare(size) == Status::Error)
          escape("unable to prepare the key");

        // actually encrypt the secret key's archive, storing the encrypted
        // portion directly into the key object, without any re-copy.
        //
        // note that since the encryption is performed with the private key,
        // the operation is equivalent to a signature.
        if (::EVP_PKEY_sign(
              this->_contexts.encrypt,
              key.region.contents,
              &size,
              buf.contents(),
              buf.size()) <= 0)
          escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

        // set the key size.
        key.region.size = size;
      }

      // (iv)
      {
        elle::Buffer buf;

        try
          {
            buf.writer() << key << data;
          }
        catch (std::exception const& err)
          {
            escape(
              "unable to serialize the asymetrically-encrypted secret key "
              "and the symetrically-encrypted data: %s",
              err.what()
                   );

          }

        // duplicate the archive's content.
        // XXX this copy is not necessary
        if (out.region.Duplicate(buf.contents(),
                                 buf.size()) == Status::Error)
          escape("unable to duplicate the archive's content");
      }

      return Status::Ok;
    }

    ///
    /// this method derives a public key according to (i) its complementary
    /// private key and (ii) the seed used for rotating this key pair.
    ///
    Status              PrivateKey::Derive(const Seed&          seed,
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
      if (K.Create(scope.key) == Status::Error)
        escape("unable to create the public key");

      return Status::Ok;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method check if two objects match.
    ///
    Boolean             PrivateKey::operator==(const PrivateKey& element) const
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
              (::BN_cmp(this->_key->pkey.rsa->d,
                        element._key->pkey.rsa->d) != 0) ||
              (::BN_cmp(this->_key->pkey.rsa->p,
                        element._key->pkey.rsa->p) != 0) ||
              (::BN_cmp(this->_key->pkey.rsa->q,
                        element._key->pkey.rsa->q) != 0))
            return false;
        }

      return true;
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(PrivateKey, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the private key internals.
    ///
    Status              PrivateKey::Dump(const Natural32        margin) const
    {
      String            alignment(margin, ' ');

      // display depending on the value.
      if (*this == PrivateKey::Null)
        {
          std::cout << alignment << "[PrivateKey] " << none << std::endl;
        }
      else
        {
          std::cout << alignment << "[PrivateKey]" << std::endl;

          // dump the internal numbers.
          std::cout << alignment << io::Dumpable::Shift << "[n] "
                    << *this->_key->pkey.rsa->n << std::endl;

          std::cout << alignment << io::Dumpable::Shift << "[d] "
                    << *this->_key->pkey.rsa->d << std::endl;

          std::cout << alignment << io::Dumpable::Shift << "[p] "
                    << *this->_key->pkey.rsa->p << std::endl;

          std::cout << alignment << io::Dumpable::Shift << "[q] "
                    << *this->_key->pkey.rsa->q << std::endl;
        }

      return Status::Ok;
    }

  }
}
