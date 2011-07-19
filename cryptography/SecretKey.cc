//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/cryptography/SecretKey.cc
//
// created       julien quintard   [thu nov  1 12:24:32 2007]
// updated       julien quintard   [fri jul 15 11:29:14 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/cryptography/SecretKey.hh>
#include <elle/cryptography/Digest.hh>
#include <elle/cryptography/OneWay.hh>

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
    const Character		SecretKey::Magic[] = "Salted__";

    ///
    /// this is the default length used when generating passwords, in bits.
    ///
    const Natural32		SecretKey::Default::Length = 512;

    ///
    /// this is the encryption algorithm used by the SecretKey class.
    ///
    const ::EVP_CIPHER*		SecretKey::Algorithms::Cipher =
      ::EVP_aes_256_cbc();

    ///
    /// this is the hash algorithm used by the encryption process.
    ///
    const ::EVP_MD*		SecretKey::Algorithms::Digest =
      ::EVP_md5();

    ///
    /// this defines a null secret key.
    ///
    const SecretKey		SecretKey::Null;

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
    Status		SecretKey::Create(const String&		password)
    {
      enter();

      // assign the password to the internal key object.
      if (this->region.Duplicate((Byte*)password.c_str(),
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
    /// the argument length represents the length of the key, in bits.
    ///
    Status		SecretKey::Generate(const Natural32	length)
    {
      Natural32		size;

      enter();

      // convert the length in a byte-specific size.
      size = length / 8;

      // prepare the password.
      if (this->region.Prepare(size) == StatusError)
	escape("unable to prepare the key");

      // generate the key.
      ::RAND_pseudo_bytes((unsigned char*)this->region.contents, size);

      // manually update the size.
      this->region.size = size;

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
      Natural32		capacity;
      int		size;
      ::EVP_CIPHER_CTX	context;

      wrap(context);
      enter(local(context, ::EVP_CIPHER_CTX_cleanup));

      // generate a salt.
      ::RAND_pseudo_bytes((unsigned char*)salt, sizeof (salt));

      // generate the key and IV based on the salt and password.
      if (::EVP_BytesToKey(SecretKey::Algorithms::Cipher,
			   SecretKey::Algorithms::Digest,
			   (unsigned char*)salt,
			   (unsigned char*)this->region.contents,
			   this->region.size,
			   1,
			   (unsigned char*)key,
			   (unsigned char*)iv) != sizeof (key))
	escape("the generated key's size does not match the one expected");

      // initialise the context.
      ::EVP_CIPHER_CTX_init(&context);

      // track the context.
      track(context);

      // initialise the ciphering process.
      if (::EVP_EncryptInit_ex(&context,
			       SecretKey::Algorithms::Cipher,
			       NULL,
			       key,
			       iv) == 0)
	escape(::ERR_error_string(ERR_get_error(), NULL));

      // retreive the cipher-specific block size.
      capacity = ::EVP_CIPHER_CTX_block_size(&context);

      // allocate the cipher.
      if (cipher.region.Prepare(sizeof (SecretKey::Magic) - 1 + sizeof (salt) +
				plain.size + capacity) == StatusError)
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

      // stop tracking the context as it will be released naturally.
      untrack(context);

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
      Natural32		capacity;
      int		size;
      ::EVP_CIPHER_CTX	context;

      wrap(context);
      enter(local(context, ::EVP_CIPHER_CTX_cleanup));

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

      // initialise the context.
      ::EVP_CIPHER_CTX_init(&context);

      // track the context since it has been initialised and therefore
      // should be released properly should an error occur.
      track(context);

      // initialise the ciphering process.
      if (::EVP_DecryptInit_ex(&context,
			       SecretKey::Algorithms::Cipher,
			       NULL,
			       key,
			       iv) == 0)
	escape(::ERR_error_string(ERR_get_error(), NULL));

      // retreive the cipher-specific block size.
      capacity = ::EVP_CIPHER_CTX_block_size(&context);

      // allocate the clear.
      if (clear.Prepare(cipher.region.size -
			(sizeof (SecretKey::Magic) - 1 + sizeof (salt)) +
			capacity) == StatusError)
	escape("unable to reserve memory for the clear text");

      // cipher the cipher text.
      if (::EVP_DecryptUpdate(&context,
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
      if (::EVP_DecryptFinal_ex(&context,
				clear.contents + size,
				&size) == 0)
	escape(::ERR_error_string(ERR_get_error(), NULL));

      // update the clear size.
      clear.size += size;

      // clean the context structure.
      ::EVP_CIPHER_CTX_cleanup(&context);

      // stop tracking the context.
      untrack(context);

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method check if two objects match.
    ///
    Boolean		SecretKey::operator==(const SecretKey&	element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	true();

      // compare the internal region.
      if (this->region != element.region)
	false();

      true();
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
    Status		SecretKey::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

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
      enter();

      // serialize the internal key.
      if (archive.Serialize(this->region) == StatusError)
	escape("unable to serialize the internal key");

      leave();
    }

    ///
    /// this method extract a secret key from the given archive.
    ///
    Status		SecretKey::Extract(Archive&		archive)
    {
      enter();

      // extract the key.
      if (archive.Extract(this->region) == StatusError)
	escape("unable to extract the internal key");

      leave();
    }

  }
}
