#include <cryptography/SecretKey.hh>
#include <cryptography/Digest.hh>
#include <cryptography/random.hh>
#include <cryptography/Cipher.hh>
#include <cryptography/cryptography.hh>

#include <elle/log.hh>
#include <elle/idiom/Open.hh>

namespace infinit
{
  namespace cryptography
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this magic is recorded in encrypted text so that the decryption process
    /// can know that it has been salted.
    ///
    const elle::Character             SecretKey::Magic[] = "Salted__";

    ///
    /// this is the default length used when generating passwords, in bits.
    ///
    const elle::Natural32             SecretKey::Default::Length = 512;

    ///
    /// this is the encryption algorithm used by the SecretKey class.
    ///
    const ::EVP_CIPHER*         SecretKey::Algorithms::Cipher =
      ::EVP_aes_256_cbc();

    ///
    /// this is the hash algorithm used by the encryption process.
    ///
    const ::EVP_MD*             SecretKey::Algorithms::Digest =
      ::EVP_md5();

    ///
    /// this defines a null secret key.
    ///
    const SecretKey             SecretKey::Null;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    SecretKey::SecretKey()
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method builds a key based on the given password.
    ///
    elle::Status              SecretKey::Create(const elle::String&         password)
    {
      // assign the password to the internal key object.
      if (this->region.Duplicate(
            reinterpret_cast<const elle::Byte*>(password.c_str()),
            password.length()) == elle::Status::Error)
        escape("unable to assign the given password to the key");

      return elle::Status::Ok;
    }

    ///
    /// this method generates a key with the default length.
    ///
    elle::Status              SecretKey::Generate()
    {
      return (this->Generate(SecretKey::Default::Length));
    }

    ///
    /// this method generates a key by generating a password.
    ///
    /// the argument length represents the length of the key, in bits.
    ///
    elle::Status              SecretKey::Generate(const elle::Natural32     length)
    {
      elle::Natural32         size;

      // convert the length in a byte-specific size.
      size = length / 8;

      // prepare the password.
      if (this->region.Prepare(size) == elle::Status::Error)
        escape("unable to prepare the key");

      // generate the key.
      // XXX[change the attribute to a buffer]
      elle::Buffer buffer(random::generate<elle::Buffer>(size));
      if (this->region.Duplicate(buffer.contents(), buffer.size()) == elle::Status::Error)
        escape("XXX");

      return elle::Status::Ok;
    }

    ///
    /// this method encrypts the given plain text.
    ///
    elle::Status SecretKey::Encrypt(elle::WeakBuffer const&  in,
                              Cipher&                           cipher) const
    {
      unsigned char     key[EVP_MAX_KEY_LENGTH];
      unsigned char     iv[EVP_MAX_IV_LENGTH];
      unsigned char     salt[PKCS5_SALT_LEN];
      elle::Natural32         capacity;

      // generate a salt.
      ::RAND_pseudo_bytes(salt, sizeof (salt));

      size_t bytes_to_key = ::EVP_BytesToKey(SecretKey::Algorithms::Cipher,
                                             SecretKey::Algorithms::Digest,
                                             salt,
                                             this->region.contents,
                                             this->region.size,
                                             1,
                                             key,
                                             iv);
      // generate the key and IV based on the salt and password.
      // XXX the test was bytes_to_key != sizeof(key)
      if (bytes_to_key > sizeof (key))
        escape("the generated key's size does not match the one expected");

      struct Scope
      {
        ::EVP_CIPHER_CTX  context;

        Scope() { ::EVP_CIPHER_CTX_init(&this->context); }
        ~Scope() { ::EVP_CIPHER_CTX_cleanup(&this->context); }
      } scope;

      // initialise the ciphering process.
      if (::EVP_EncryptInit_ex(&scope.context,
                               SecretKey::Algorithms::Cipher,
                               nullptr,
                               key,
                               iv) == 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      // retreive the cipher-specific block size.
      capacity = ::EVP_CIPHER_CTX_block_size(&scope.context);

      // allocate the cipher.
      cipher.buffer().size(sizeof (SecretKey::Magic) -
                           1 +
                           sizeof (salt) +
                           in.size() +
                           capacity);

      // push the magic string directly into the cipher.
      ::memcpy(cipher.buffer().mutable_contents(),
               SecretKey::Magic,
               sizeof (SecretKey::Magic) - 1);

      // push the salt directly into the cipher.
      ::memcpy(cipher.buffer().mutable_contents() +
               sizeof (SecretKey::Magic) - 1,
               salt,
               sizeof (salt));

      // initialise the cipher's size.
      int size_header(sizeof (SecretKey::Magic) - 1 + sizeof (salt));

      int size_update(0);

      // cipher the plain text.
      if (::EVP_EncryptUpdate(&scope.context,
                              cipher.buffer().mutable_contents() +
                              size_header,
                              &size_update,
                              in.contents(),
                              in.size()) == 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      int size_finalize(0);

      // finialise the ciphering process.
      if (::EVP_EncryptFinal_ex(&scope.context,
                                cipher.buffer().mutable_contents() +
                                size_header + size_update,
                                &size_finalize) == 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      // update the cipher size.
      cipher.buffer().size(size_header + size_update + size_finalize);

      return elle::Status::Ok;
    }

    ///
    /// this method decrypts the given cipher.
    ///
    elle::Status SecretKey::Decrypt(const Cipher&             cipher,
                              elle::Buffer&    out) const
    {
      unsigned char     key[EVP_MAX_KEY_LENGTH];
      unsigned char     iv[EVP_MAX_IV_LENGTH];
      unsigned char     salt[PKCS5_SALT_LEN];
      elle::Natural32         capacity;

      // check whether the cipher was produced with a salt.
      if (::memcmp(SecretKey::Magic,
                   cipher.buffer().contents(),
                   sizeof (SecretKey::Magic) - 1) != 0)
        escape("this encrypted information was produced without any salt");

      // copy the salt for the sack of clarity.
      ::memcpy(salt,
               cipher.buffer().contents() + sizeof (Magic) - 1,
               sizeof (salt));

      size_t bytes_to_key = ::EVP_BytesToKey(SecretKey::Algorithms::Cipher,
                                             SecretKey::Algorithms::Digest,
                                             salt,
                                             this->region.contents,
                                             this->region.size,
                                             1,
                                             key,
                                             iv);
      // generate the key and IV based on the salt and password.
      if (bytes_to_key > sizeof (key)) // XXX the test was bytes_to_key != sizeof(key)
          escape("the generated key's size does not match the one expected");

      struct Scope
      {
        ::EVP_CIPHER_CTX  context;
        Scope() { ::EVP_CIPHER_CTX_init(&this->context); }
        ~Scope() { ::EVP_CIPHER_CTX_cleanup(&this->context); }
      } scope;

      // initialise the ciphering process.
      if (::EVP_DecryptInit_ex(&scope.context,
                               SecretKey::Algorithms::Cipher,
                               nullptr,
                               key,
                               iv) == 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      // retreive the cipher-specific block size.
      capacity = ::EVP_CIPHER_CTX_block_size(&scope.context);

      // allocate the out buffer.
      out.size(cipher.buffer().size() -
               (sizeof (SecretKey::Magic) - 1 + sizeof (salt)) +
               capacity);

      int size_update(0);

      if (::EVP_DecryptUpdate(&scope.context,
                              out.mutable_contents(),
                              &size_update,
                              cipher.buffer().contents() +
                              sizeof (SecretKey::Magic) - 1 +
                              sizeof (salt),
                              cipher.buffer(.size() -
                              (sizeof (SecretKey::Magic) - 1 +
                               sizeof (salt))) == 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      int size_final(0);

      // finalise the ciphering process.
      if (::EVP_DecryptFinal_ex(&scope.context,
                                out.mutable_contents() + size_update,
                                &size_final) == 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      // update the clear size.
      out.size(size_update + size_final);

      return elle::Status::Ok;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method check if two objects match.
    ///
    elle::Boolean             SecretKey::operator==(const SecretKey&  element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return true;

      // compare the internal region.
      return (this->region == element.region);
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(SecretKey, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the secret key internals.
    ///
    elle::Status              SecretKey::Dump(const elle::Natural32         margin) const
    {
      elle::String            alignment(margin, ' ');

      // display the key depending on its value.
      if (*this == SecretKey::Null)
        {
          std::cout << alignment << "[SecretKey] " << elle::none << std::endl;
        }
      else
        {
          std::cout << alignment << "[SecretKey] " << std::endl;

          // dump the region.
          if (this->region.Dump(margin + 2) == elle::Status::Error)
            escape("unable to dump the secret key");
        }

      return elle::Status::Ok;
    }

  }
}
