#include <elle/cryptography/SecretKey.hh>
#include <elle/cryptography/Digest.hh>
#include <elle/cryptography/OneWay.hh>
#include <elle/cryptography/Random.hh>
#include <elle/cryptography/Cipher.hh>

#include <elle/log.hh>
#include <elle/idiom/Open.hh>

namespace elle
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
    const Character             SecretKey::Magic[] = "Salted__";

    ///
    /// this is the default length used when generating passwords, in bits.
    ///
    const Natural32             SecretKey::Default::Length = 512;

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
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method builds a key based on the given password.
    ///
    Status              SecretKey::Create(const String&         password)
    {
      // assign the password to the internal key object.
      if (this->region.Duplicate(
            reinterpret_cast<const Byte*>(password.c_str()),
            password.length()) == Status::Error)
        escape("unable to assign the given password to the key");

      return Status::Ok;
    }

    ///
    /// this method generates a key with the default length.
    ///
    Status              SecretKey::Generate()
    {
      return (this->Generate(SecretKey::Default::Length));
    }

    ///
    /// this method generates a key by generating a password.
    ///
    /// the argument length represents the length of the key, in bits.
    ///
    Status              SecretKey::Generate(const Natural32     length)
    {
      Natural32         size;

      // convert the length in a byte-specific size.
      size = length / 8;

      // prepare the password.
      if (this->region.Prepare(size) == Status::Error)
        escape("unable to prepare the key");

      // generate the key.
      if (Random::Generate(this->region, size) == Status::Error)
        escape("unable to generate the region");

      return Status::Ok;
    }

    ///
    /// this method encrypts the given plain text.
    ///
    Status SecretKey::Encrypt(elle::WeakBuffer const&  in,
                              Cipher&                           cipher) const
    {
      unsigned char     key[EVP_MAX_KEY_LENGTH];
      unsigned char     iv[EVP_MAX_IV_LENGTH];
      unsigned char     salt[PKCS5_SALT_LEN];
      Natural32         capacity;
      int               size;

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
      if (cipher.region.Prepare(sizeof (SecretKey::Magic) -
                                1 +
                                sizeof (salt) +
                                in.size() +
                                capacity) == Status::Error)
        escape("unable to reserve memory for the cipher");

      // push the magic string directly into the cipher.
      ::memcpy(cipher.region.contents,
               SecretKey::Magic,
               sizeof (SecretKey::Magic) - 1);

      // push the salt directly into the cipher.
      ::memcpy(cipher.region.contents + sizeof (SecretKey::Magic) - 1,
               salt,
               sizeof (salt));

      // initialise the cipher's size.
      cipher.region.size = sizeof (SecretKey::Magic) - 1 + sizeof (salt);

      // cipher the plain text.
      if (::EVP_EncryptUpdate(&scope.context,
                              cipher.region.contents + cipher.region.size,
                              &size,
                              in.contents(),
                              in.size()) == 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      // update the cipher size.
      cipher.region.size += size;

      // finialise the ciphering process.
      if (::EVP_EncryptFinal_ex(&scope.context,
                                cipher.region.contents + cipher.region.size,
                                &size) == 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      // update the cipher size.
      cipher.region.size += size;

      return Status::Ok;
    }

    ///
    /// this method decrypts the given cipher.
    ///
    Status SecretKey::Decrypt(const Cipher&             cipher,
                              elle::Buffer&    out) const
    {
      unsigned char     key[EVP_MAX_KEY_LENGTH];
      unsigned char     iv[EVP_MAX_IV_LENGTH];
      unsigned char     salt[PKCS5_SALT_LEN];
      Natural32         capacity;
      int               size;

      // check whether the cipher was produced with a salt.
      if (::memcmp(SecretKey::Magic,
                   cipher.region.contents,
                   sizeof (SecretKey::Magic) - 1) != 0)
        escape("this encrypted information was produced without any salt");

      // copy the salt for the sack of clarity.
      ::memcpy(salt,
               cipher.region.contents + sizeof (Magic) - 1,
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

      // allocate the out buffer
      out.size(
          cipher.region.size -
          (sizeof (SecretKey::Magic) - 1 + sizeof (salt)) +
          capacity
      );

      if (::EVP_DecryptUpdate(&scope.context,
                              out.mutable_contents(),
                              &size,
                              cipher.region.contents +
                              sizeof (SecretKey::Magic) - 1 +
                              sizeof (salt),
                              cipher.region.size -
                              (sizeof (SecretKey::Magic) - 1 +
                               sizeof (salt))) == 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      // update the clear size.
      out.size(out.size() + size);

      // finalise the ciphering process.
      if (::EVP_DecryptFinal_ex(&scope.context,
                                out.mutable_contents() + size,
                                &size) == 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      // update the clear size.
      out.size(out.size() + size);

      return Status::Ok;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method check if two objects match.
    ///
    Boolean             SecretKey::operator==(const SecretKey&  element) const
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
    Status              SecretKey::Dump(const Natural32         margin) const
    {
      String            alignment(margin, ' ');

      // display the key depending on its value.
      if (*this == SecretKey::Null)
        {
          std::cout << alignment << "[SecretKey] " << none << std::endl;
        }
      else
        {
          std::cout << alignment << "[SecretKey] " << std::endl;

          // dump the region.
          if (this->region.Dump(margin + 2) == Status::Error)
            escape("unable to dump the secret key");
        }

      return Status::Ok;
    }

  }
}
