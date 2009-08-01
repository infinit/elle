//
// ---------- header ----------------------------------------------------------
//
// project       il
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/crypto/SecretKey.cc
//
// created       julien quintard   [thu nov  1 12:24:32 2007]
// updated       julien quintard   [sat aug  1 15:42:07 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/crypto/SecretKey.hh>

namespace elle
{
  using namespace io;
  using namespace core;
  using namespace misc;
  using namespace archive;

  namespace crypto
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this magic is recorded in encrypted text so that the decryption process
    /// can know that it has been salted.
    ///
    const Character		SecretKey::Magic[] = "Salted__";

    ///
    /// this is the default length used when generating passwords.
    ///
    const Natural32		SecretKey::Default::Length = 32;

    ///
    /// this is the encryption algorithm used by the SecretKey class.
    ///
    const ::EVP_CIPHER*		SecretKey::Algorithms::Cipher = ::EVP_aes_256_cbc();

    ///
    /// this is the hash algorithm used by the encryption process.
    ///
    const ::EVP_MD*		SecretKey::Algorithms::Digest = ::EVP_md5();

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method builds a key based on the given password.
    ///
    Status		SecretKey::Create(const String&		password)
    {
      // assign the password to the internal key object.
      if (this->key.Duplicate((Byte*)password.c_str(),
			      password.length()) == StatusError)
	escape("unable to assign the given password to the key");

      leave();
    }

    ///
    /// this method generates a key with the default length.
    ///
    Status		SecretKey::Generate()
    {
      return (this->Generate(SecretKey::Default::Length));
    }

    ///
    /// this method generates a key by generating a password.
    ///
    Status		SecretKey::Generate(const Natural32	length)
    {
      // prepare the password.
      if (this->key.Prepare(length) == StatusError)
	escape("unable to prepare the key");

      // generate the key.
      ::RAND_pseudo_bytes((unsigned char*)this->key.contents,
			  length);

      // manually update the size.
      this->key.size = length;

      leave();
    }

    ///
    /// this method encrypts the given plain text.
    ///
    Status		SecretKey::Encrypt(const Plain&		plain,
					   Cipher&		cipher) const
    {
      Byte		key[EVP_MAX_KEY_LENGTH];
      Byte		iv[EVP_MAX_IV_LENGTH];
      Byte		salt[PKCS5_SALT_LEN];
      Natural32		length;
      int		size;
      ::EVP_CIPHER_CTX	context;

      // generate a salt.
      ::RAND_pseudo_bytes((unsigned char*)salt, sizeof(salt));

      // generate the key and IV based on the salt and password.
      if (::EVP_BytesToKey(SecretKey::Algorithms::Cipher,
			   SecretKey::Algorithms::Digest,
			   (unsigned char*)salt,
			   (unsigned char*)this->key.contents,
			   this->key.size,
			   1,
			   (unsigned char*)key,
			   (unsigned char*)iv) != sizeof(key))
	escape("the generated key's size does not match the one expected");

      // initialise the context.
      ::EVP_CIPHER_CTX_init(&context);

      // initialise the ciphering process.
      if (::EVP_EncryptInit_ex(&context,
			       SecretKey::Algorithms::Cipher,
			       NULL,
			       key,
			       iv) == 0)
	escape(::ERR_error_string(ERR_get_error(), NULL));

      // retreive the cipher-specific block length.
      length = ::EVP_CIPHER_CTX_block_size(&context);

      // allocate the cipher.
      if (cipher.region.Prepare(sizeof(SecretKey::Magic) - 1 + sizeof(salt) +
				plain.size + length - 1) == StatusError)
	escape("unable to reserve memory for the cipher");

      // push the magic string directly into the cipher.
      ::memcpy(cipher.region.contents,
	       SecretKey::Magic,
	       sizeof(SecretKey::Magic) - 1);

      // push the salt directly into the cipher.
      ::memcpy(cipher.region.contents + sizeof(SecretKey::Magic) - 1,
	       salt,
	       sizeof(salt));

      // initialise the cipher's size.
      cipher.region.size = sizeof(SecretKey::Magic) - 1 + sizeof(salt);

      // cipher the plain text.
      if (::EVP_EncryptUpdate(&context,
			      cipher.region.contents + cipher.region.size,
			      &size,
			      plain.contents,
			      plain.size) == 0)
	escape(::ERR_error_string(ERR_get_error(), NULL));

      // update the cipher size.
      cipher.region.size += size;

      // finialise the ciphering process.
      if (::EVP_EncryptFinal_ex(&context,
				cipher.region.contents + cipher.region.size,
				&size) == 0)
	escape(::ERR_error_string(ERR_get_error(), NULL));

      // update the cipher size.
      cipher.region.size += size;

      // clean the context structure.
      ::EVP_CIPHER_CTX_cleanup(&context);

      leave();
    }

    ///
    /// this method decrypts the given cipher.
    ///
    Status		SecretKey::Decrypt(const Cipher&	cipher,
					   Clear&		clear) const
    {
      Byte		key[EVP_MAX_KEY_LENGTH];
      Byte		iv[EVP_MAX_IV_LENGTH];
      Byte		salt[PKCS5_SALT_LEN];
      Natural32		length;
      int		size;
      ::EVP_CIPHER_CTX	context;

      // check whether the cipher was produced with a salt.
      if (::memcmp(SecretKey::Magic,
		   cipher.region.contents,
		   sizeof(SecretKey::Magic) - 1) != 0)
	escape("this encrypted information was produced without any salt");

      // copy the salt for the sack of clarity.
      ::memcpy(salt,
	       cipher.region.contents + sizeof(Magic) - 1,
	       sizeof(salt));

      // generate the key and IV based on the salt and password.
      if (::EVP_BytesToKey(SecretKey::Algorithms::Cipher,
			   SecretKey::Algorithms::Digest,
			   salt,
			   this->key.contents,
			   this->key.size,
			   1,
			   key,
			   iv) != sizeof(key))
	escape("the generated key's size does not match the one expected");

      // initialise the context.
      ::EVP_CIPHER_CTX_init(&context);

      // initialise the ciphering process.
      if (::EVP_DecryptInit_ex(&context,
			       SecretKey::Algorithms::Cipher,
			       NULL,
			       key,
			       iv) == 0)
	escape(::ERR_error_string(ERR_get_error(), NULL));

      // retreive the cipher-specific block length.
      length = ::EVP_CIPHER_CTX_block_size(&context);

      // allocate the clear.
      if (clear.Prepare(cipher.region.size -
			(sizeof(SecretKey::Magic) - 1 + sizeof(salt)) +
			length) == StatusError)
	escape("unable to reserve memory for the clear text");

      // cipher the cipher text.
      if (::EVP_DecryptUpdate(&context,
			      clear.contents,
			      &size,
			      cipher.region.contents +
			      sizeof(SecretKey::Magic) - 1 +
			      sizeof(salt),
			      cipher.region.size -
			      (sizeof(SecretKey::Magic) - 1 +
			       sizeof(salt))) == 0)
	escape(::ERR_error_string(ERR_get_error(), NULL));

      // update the clear size.
      clear.size += size;

      // finalise the ciphering process.
      if (::EVP_DecryptFinal_ex(&context,
				clear.contents + size,
				&size) == 0)
	escape(::ERR_error_string(ERR_get_error(), NULL));

      // update the clear size.
      clear.size += size;

      // clean the context structure.
      ::EVP_CIPHER_CTX_cleanup(&context);

      leave();
    }

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// assign the secret key.
    ///
    SecretKey&		SecretKey::operator=(const SecretKey&	element)
    {
      // self-check.
      if (this == &element)
	return (*this);

      // recycle the secret key.
      if (this->Recycle<SecretKey>(&element) == StatusError)
	yield("unable to recycle the secret key", *this);

      return (*this);
    }

    ///
    /// this method check if two objects match.
    ///
    Boolean		SecretKey::operator==(const SecretKey&	element)
    {
      // compare the internal region.
      return (this->key == element.key);
    }

    ///
    /// this method checks if two objects dis-match.
    ///
    Boolean		SecretKey::operator!=(const SecretKey&	element)
    {
      return (!(*this == element));
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the secret key internals.
    ///
    Status		SecretKey::Dump(const Natural32		margin)
    {
      String		alignment(margin, ' ');

      std::cout << alignment << "[SecretKey]" << std::endl;

      if (this->key.Dump(margin + 2) == StatusError)
	escape("unable to dump the secret key");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes a secret key object.
    ///
    Status		SecretKey::Serialize(Archive&		archive) const
    {
      // serialize the internal key.
      if (archive.Serialize(this->key) == StatusError)
	escape("unable to serialize the internal key");

      leave();
    }

    ///
    /// this method extract a secret key from the given archive.
    ///
    Status		SecretKey::Extract(Archive&		archive)
    {
      // extract the key.
      if (archive.Extract(this->key) == StatusError)
	escape("unable to extract the internal key");

      leave();
    }

  }
}
