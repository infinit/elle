//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [thu nov  1 12:24:32 2007]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/cryptography/SecretKey.hh>
#include <elle/cryptography/Digest.hh>
#include <elle/cryptography/OneWay.hh>
#include <elle/cryptography/Random.hh>

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
            password.length()) == StatusError)
        escape("unable to assign the given password to the key");

      return elle::StatusOk;
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
      if (this->region.Prepare(size) == StatusError)
        escape("unable to prepare the key");

      // generate the key.
      if (Random::Generate(this->region, size) == StatusError)
        escape("unable to generate the region");

      return elle::StatusOk;
    }

    ///
    /// this method encrypts the given plain text.
    ///
    Status              SecretKey::Encrypt(const Plain&         plain,
                                           Cipher&              cipher) const
    {
      unsigned char     key[EVP_MAX_KEY_LENGTH];
      unsigned char     iv[EVP_MAX_IV_LENGTH];
      unsigned char     salt[PKCS5_SALT_LEN];
      Natural32         capacity;
      int               size;

      // generate a salt.
      ::RAND_pseudo_bytes(salt, sizeof (salt));

      // generate the key and IV based on the salt and password.
      if (::EVP_BytesToKey(SecretKey::Algorithms::Cipher,
                           SecretKey::Algorithms::Digest,
                           salt,
                           this->region.contents,
                           this->region.size,
                           1,
                           key,
                           iv) != sizeof (key))
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
                               NULL,
                               key,
                               iv) == 0)
        escape(::ERR_error_string(ERR_get_error(), NULL));

      // retreive the cipher-specific block size.
      capacity = ::EVP_CIPHER_CTX_block_size(&scope.context);

      // allocate the cipher.
      if (cipher.region.Prepare(sizeof (SecretKey::Magic) -
                                1 +
                                sizeof (salt) +
                                plain.size +
                                capacity) == StatusError)
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
                              plain.contents,
                              plain.size) == 0)
        escape(::ERR_error_string(ERR_get_error(), NULL));

      // update the cipher size.
      cipher.region.size += size;

      // finialise the ciphering process.
      if (::EVP_EncryptFinal_ex(&scope.context,
                                cipher.region.contents + cipher.region.size,
                                &size) == 0)
        escape(::ERR_error_string(ERR_get_error(), NULL));

      // update the cipher size.
      cipher.region.size += size;

      return elle::StatusOk;
    }

    ///
    /// this method decrypts the given cipher.
    ///
    Status              SecretKey::Decrypt(const Cipher&        cipher,
                                           Clear&               clear) const
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

      // generate the key and IV based on the salt and password.
      if (::EVP_BytesToKey(SecretKey::Algorithms::Cipher,
                           SecretKey::Algorithms::Digest,
                           salt,
                           this->region.contents,
                           this->region.size,
                           1,
                           key,
                           iv) != sizeof (key))
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
                               NULL,
                               key,
                               iv) == 0)
        escape(::ERR_error_string(ERR_get_error(), NULL));

      // retreive the cipher-specific block size.
      capacity = ::EVP_CIPHER_CTX_block_size(&scope.context);

      // allocate the clear.
      if (clear.Prepare(cipher.region.size -
                        (sizeof (SecretKey::Magic) - 1 + sizeof (salt)) +
                        capacity) == StatusError)
        escape("unable to reserve memory for the clear text");

      // cipher the cipher text.
      if (::EVP_DecryptUpdate(&scope.context,
                              clear.contents,
                              &size,
                              cipher.region.contents +
                              sizeof (SecretKey::Magic) - 1 +
                              sizeof (salt),
                              cipher.region.size -
                              (sizeof (SecretKey::Magic) - 1 +
                               sizeof (salt))) == 0)
        escape(::ERR_error_string(ERR_get_error(), NULL));

      // update the clear size.
      clear.size += size;

      // finalise the ciphering process.
      if (::EVP_DecryptFinal_ex(&scope.context,
                                clear.contents + size,
                                &size) == 0)
        escape(::ERR_error_string(ERR_get_error(), NULL));

      // update the clear size.
      clear.size += size;

      return elle::StatusOk;
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
        return elle::StatusTrue;

      // compare the internal region.
      if (this->region != element.region)
        return elle::StatusFalse;

      return elle::StatusTrue;
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
          if (this->region.Dump(margin + 2) == StatusError)
            escape("unable to dump the secret key");
        }

      return elle::StatusOk;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes a secret key object.
    ///
    Status              SecretKey::Serialize(Archive&           archive) const
    {
      // serialize the internal key.
      if (archive.Serialize(this->region) == StatusError)
        escape("unable to serialize the internal key");

      return elle::StatusOk;
    }

    ///
    /// this method extract a secret key from the given archive.
    ///
    Status              SecretKey::Extract(Archive&             archive)
    {
      // extract the key.
      if (archive.Extract(this->region) == StatusError)
        escape("unable to extract the internal key");

      return elle::StatusOk;
    }

  }
}
