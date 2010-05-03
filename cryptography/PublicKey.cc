//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/cryptography/PublicKey.cc
//
// created       julien quintard   [tue oct 30 01:23:20 2007]
// updated       julien quintard   [mon may  3 22:50:15 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/cryptography/PublicKey.hh>
#include <elle/cryptography/Digest.hh>
#include <elle/cryptography/OneWay.hh>
#include <elle/cryptography/SecretKey.hh>

namespace elle
{
  namespace cryptography
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this defines a null public key.
    ///
    const PublicKey		PublicKey::Null;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// this method initializes the object.
    ///
    PublicKey::PublicKey():
      key(NULL)
    {
      // initialize the contexts.
      this->contexts.encrypt = NULL;
      this->contexts.verify = NULL;
    }

    ///
    /// this is the copy constructor.
    ///
    PublicKey::PublicKey(const PublicKey&			K)
    {
      // re-create the public key by duplicate the internal numbers.
      if (this->Create(K.key) == StatusError)
	fail("unable to duplicate the public key");
    }

    ///
    /// this method releases the resources.
    ///
    PublicKey::~PublicKey()
    {
      // release the resources.
      if (this->key != NULL)
	::EVP_PKEY_free(this->key);

      if (this->contexts.encrypt != NULL)
	::EVP_PKEY_CTX_free(this->contexts.encrypt);

      if (this->contexts.verify != NULL)
	::EVP_PKEY_CTX_free(this->contexts.verify);
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method constructs a valid public key.
    ///
    Status		PublicKey::Create(const ::EVP_PKEY*	key)
    {
      enter();

      // call the creation method.
      if (this->Create(::BN_dup(key->pkey.rsa->n),
		       ::BN_dup(key->pkey.rsa->e)) == StatusError)
	escape("unable to create the public key");

      leave();
    }

    ///
    /// this method constructs a valid public key given the proper
    /// numbers.
    ///
    Status		PublicKey::Create(Large*		n,
					  Large*		e)
    {
      ::RSA*		rsa;

      enter(slab(rsa, ::RSA_free));

      //
      // key
      //

      // initialise the public key structure.
      this->key = ::EVP_PKEY_new();

      // create the RSA structure.
      if ((rsa = ::RSA_new()) == NULL)
	escape(::ERR_error_string(ERR_get_error(), NULL));

      // assign the big numbers relevant to the public key.
      rsa->n = n;
      rsa->e = e;

      // set the rsa structure into the public key.
      if (::EVP_PKEY_assign_RSA(this->key, rsa) <= 0)
	escape(::ERR_error_string(ERR_get_error(), NULL));

      // stop keeping track of this variable as it has been
      // merged with another one.
      waive(rsa);

      //
      // contexts
      //

      // create, initialize and configure---by setting the padding---the
      // encrypt context.
      if ((this->contexts.encrypt = ::EVP_PKEY_CTX_new(this->key, NULL)) ==
	  NULL)
	escape(::ERR_error_string(ERR_get_error(), NULL));

      if (::EVP_PKEY_encrypt_init(this->contexts.encrypt) <= 0)
	escape(::ERR_error_string(ERR_get_error(), NULL));

      if (::EVP_PKEY_CTX_ctrl(this->contexts.encrypt,
			      EVP_PKEY_RSA,
			      -1,
			      EVP_PKEY_CTRL_RSA_PADDING,
			      RSA_PKCS1_OAEP_PADDING,
			      NULL) <= 0)
	escape(::ERR_error_string(ERR_get_error(), NULL));

      // create and initialize the verify context.
      if ((this->contexts.verify = EVP_PKEY_CTX_new(this->key, NULL)) == NULL)
        escape(::ERR_error_string(ERR_get_error(), NULL));

      if (::EVP_PKEY_verify_init(this->contexts.verify) <= 0)
        escape(::ERR_error_string(ERR_get_error(), NULL));

      leave();
    }

    ///
    /// this method encrypts the given data with the public key.
    ///
    /// since (i) the public key size limits the size of the data that
    /// can be encrypted (ii) raising large data to large exponent
    /// is very slow; the algorithm below consists in (i) generating
    /// a secret key, (ii) ciphering the plain text with this key,
    /// (iii) encrypting the secret key with the public key and finally
    /// (iv) returning an archive containing the asymetrically-encrypted
    /// secret key with the symmetrically-encrypted data.
    ///
    Status		PublicKey::Encrypt(const Plain&		plain,
					   Code&		code) const
    {
      SecretKey		secret;

      Code		key;
      Cipher		data;

      enter();

      // (i)
      {
	// generate a secret key.
	if (secret.Generate() == StatusError)
	  escape("unable to generate the secret key");
      }

      // (ii)
      {
	// cipher the plain text with the secret key.
	if (secret.Encrypt(plain, data) == StatusError)
	  escape("unable to cipher the plain text with the secret key");
      }

      // (iii)
      {
	Archive		archive;
	size_t		size;

	// first, create an archive.
	if (archive.Create() == StatusError)
	  escape("unable to create an achive");

	// then, serialize the secret key.
	if (archive.Serialize(secret) == StatusError)
	  escape("unable to serialize the secret key");

	// compute the size of the archived symmetric key.
	if (::EVP_PKEY_encrypt(this->contexts.encrypt,
			       NULL,
			       &size,
			       (const unsigned char*)archive.contents,
			       archive.size) <= 0)
	  escape(::ERR_error_string(ERR_get_error(), NULL));

	// allocate memory so the key can receive the upcoming
	// encrypted portion.
	if (key.region.Prepare(size) == StatusError)
	  escape("unable to prepare the key");

	// actually encrypt the secret key's archive, storing the encrypted
	// portion directly into the key object, without any re-copy.
	if (::EVP_PKEY_encrypt(this->contexts.encrypt,
			       (unsigned char*)key.region.contents,
			       &size,
			       (const unsigned char*)archive.contents,
			       archive.size) <= 0)
	  escape(::ERR_error_string(ERR_get_error(), NULL));

	// set the key size.
	key.region.size = size;
      }

      // (iv)
      {
	Archive		archive;

	// create the main archive.
	if (archive.Create() == StatusError)
	  escape("unable to create the archive");

	// serialize the key.
	if (archive.Serialize(key, data) == StatusError)
	  escape("unable to serialize the asymetrically-encrypted secret key "
		 "and the symetrically-encrypted data");

	// wrap and return into the code.
	if (code.region.Acquire(archive.contents, archive.size) == StatusError)
	  escape("unable to wrap and return the archive's contents");

	// detach the data from the archive so that the data does not get
	// released.
	if (archive.Detach() == StatusError)
	  escape("unable to detach the data from the archive");
      }

      leave();
    }

    ///
    /// this method verifies that the non-signed text is equal to the
    /// plain text.
    ///
    /// note that, as for the Sign() method, this method computes
    /// the plain's digest before forwarding to the other Verify()
    /// method.
    ///
    Status		PublicKey::Verify(const Signature&	signature,
					  const Plain&		plain) const
    {
      Digest		digest;

      enter();

      // compute the plain's digest.
      if (OneWay::Hash(plain, digest) == StatusError)
	flee("unable to hash the plain");

      // verify.
      if (::EVP_PKEY_verify(this->contexts.verify,
			    (const unsigned char*)signature.region.contents,
			    signature.region.size,
			    (const unsigned char*)digest.region.contents,
			    digest.region.size) <= 0)
        flee(::ERR_error_string(ERR_get_error(), NULL));

      true();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method checks if two objects match.
    ///
    Boolean		PublicKey::operator==(const PublicKey&	element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	true();

      // if one of the key is null....
      if ((this->key == NULL) || (element.key == NULL))
	{
	  // compare the addresses.
	  if (this->key != element.key)
	    false();
	}
      else
	{
	  // compare the internal numbers.
	  if ((::BN_cmp(this->key->pkey.rsa->n,
			element.key->pkey.rsa->n) != 0) ||
	      (::BN_cmp(this->key->pkey.rsa->e,
			element.key->pkey.rsa->e) != 0))
	    false();
	}

      true();
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(PublicKey, _(), _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the public key internals.
    ///
    Status		PublicKey::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      // display depending on the value.
      if (*this == PublicKey::Null)
	{
	  std::cout << alignment << "[PublicKey] " << none << std::endl;
	}
      else
	{
	  std::cout << alignment << "[PublicKey] " << *this << std::endl;
	}

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes a public key object.
    ///
    Status		PublicKey::Serialize(Archive&		archive) const
    {
      enter();

      // serialize the internal numbers.
      if (archive.Serialize(*this->key->pkey.rsa->n,
			    *this->key->pkey.rsa->e) == StatusError)
	escape("unable to serialize the internal numbers");

      leave();
    }

    ///
    /// this method extract a public key from the given archive.
    ///
    Status		PublicKey::Extract(Archive&		archive)
    {
      Large		n;
      Large		e;

      wrap(n);
      wrap(e);
      enter(local(n, ::BN_free),
	    local(e, ::BN_free));

      // extract the numbers.
      if (archive.Extract(n, e) == StatusError)
	escape("unable to extract the internal numbers");

      // track the local variables.
      track(n);
      track(e);

      // create the EVP_PKEY object from the extract numbers.
      if (this->Create(::BN_dup(&n),
		       ::BN_dup(&e)) == StatusError)
	escape("unable to create the public key from the archive");

      // release the internal used memory.
      ::BN_clear_free(&n);
      ::BN_clear_free(&e);

      // stop tracking since those variables are going to be naturally
      // released soon.
      untrack(n);
      untrack(e);

      leave();
    }

  }
}
