//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/crypto/PublicKey.cc
//
// created       julien quintard   [tue oct 30 01:23:20 2007]
// updated       julien quintard   [thu jul 30 19:03:39 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/crypto/PublicKey.hh>

namespace elle
{
  using namespace io;
  using namespace core;
  using namespace misc;
  using namespace archive;

  namespace crypto
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// this method initializes the object.
    ///
    PublicKey::PublicKey()
    {
      this->key = NULL;

      this->contexts.encrypt = NULL;
      this->contexts.verify = NULL;
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

      //
      // key
      //

      // initialise the public key structure.
      this->key = ::EVP_PKEY_new();

      // create the RSA structure.
      if ((rsa = ::RSA_new()) == NULL)
	escape(::ERR_error_string(ERR_get_error(), NULL));

      // duplicate the big numbers relevant to the public key.
      rsa->n = n;
      rsa->e = e;

      // set the rsa structure into the public key.
      if (::EVP_PKEY_set1_RSA(this->key, rsa) <= 0)
	escape(::ERR_error_string(ERR_get_error(), NULL));

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

      Archive		archive;

      Code		key;
      Cipher		data;

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
	// create the main archive.
	if (archive.Create() == StatusError)
	  escape("unable to create the archive");

	// serialize the key.
	if (archive.Serialize(key) == StatusError)
	  escape("unable to serialize the asymetrically-encrypted secret key");

	// serialize the data.
	if (archive.Serialize(data) == StatusError)
	  escape("unable to serialize the symetrically-encrypted data");

	// detach the data from the archive so that the data can be
	// returned into the code without any copy.
	if (archive.Detach() == StatusError)
	  escape("unable to detach the data from the archive");

	// wrap and return into the code.
	if (code.region.Wrap(archive.contents, archive.size) == StatusError)
	  escape("unable to wrap and return the archive's contents");
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
// ---------- entity ----------------------------------------------------------
//

    ///
    /// assign the publickey.
    ///
    PublicKey&		PublicKey::operator=(const PublicKey&	element)
    {
      // self-check.
      if (this == &element)
	return (*this);

      // recycle the public key.
      if (this->Recycle<PublicKey>() == StatusError)
	yield("unable to recycle the public key", *this);

      // re-create the public key by duplicate the internal numbers.
      if (this->Create(element.key) == StatusError)
	yield("unable to duplicate the public key", *this);

      return (*this);
    }

    ///
    /// this method checks if two objects match.
    ///
    Boolean		PublicKey::operator==(const PublicKey&	element)
    {
      // compare the internal numbers.
      if ((::BN_cmp(this->key->pkey.rsa->n, element.key->pkey.rsa->n) != 0) ||
	  (::BN_cmp(this->key->pkey.rsa->e, element.key->pkey.rsa->e) != 0))
	false();

      true();
    }

    ///
    /// this method checks if two objects dis-match.
    ///
    Boolean		PublicKey::operator!=(const PublicKey&	element)
    {
      return (!(*this == element));
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the public key internals.
    ///
    Status		PublicKey::Dump(const Natural32		margin)
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      std::cout << alignment << "[PublicKey]" << std::endl;
      std::cout << alignment << shift << "[n] "
		<< *this->key->pkey.rsa->n << std::endl;
      std::cout << alignment << shift << "[e] "
		<< *this->key->pkey.rsa->e << std::endl;

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
      // serialize the internal numbers.
      if (archive.Serialize(*this->key->pkey.rsa->n) == StatusError)
	escape("unable to serialize 'n'");

      if (archive.Serialize(*this->key->pkey.rsa->e) == StatusError)
	escape("unable to serialize 'e'");

      leave();
    }

    ///
    /// this method extract a public key from the given archive.
    ///
    Status		PublicKey::Extract(Archive&		archive)
    {
      Large*		n;
      Large*		e;

      // allocate the new big numbers.
      if ((n = ::BN_new()) == NULL)
	escape(::ERR_error_string(ERR_get_error(), NULL));

      if ((e = ::BN_new()) == NULL)
	escape(::ERR_error_string(ERR_get_error(), NULL));

      // extract the numbers.
      if (archive.Extract(*n) == StatusError)
	escape("unable to extract 'n'");

      if (archive.Extract(*e) == StatusError)
	escape("unable to extract 'e'");

      // create the EVP_PKEY object from the extract numbers.
      if (this->Create(n, e) == StatusError)
	escape("unable to create the public key from the archive");

      leave();
    }

//
// ---------- variadic methods ------------------------------------------------
//

    ///
    /// these methods make it easier to decrypt/sign multiple items at
    /// the same time while keeping a way to catch errors.
    ///
    /// note that the code is replicated in order to provide optimisation.
    /// Indeed, otherwise, everytime a single item is hashed, the whole 9-step
    /// ifs would be executed, testing if there are more than one item
    /// to hash.
    ///

    ///
    /// encrypt
    ///

    ///
    /// this method takes any object that implements the Archivable
    /// interface and encrypts it.
    ///
    /// the process obviously consists in serializing the object first.
    ///
    /// note that Encrypt() methods return an archive hidden in the
    /// code.
    ///
    Status		PublicKey::Encrypt(const Archivable&	o,
					   Code&		code) const
    {
      Archive		archive;

      // create an archive.
      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      // serialize the object.
      if (archive.Serialize(o) == StatusError)
	escape("unable to serialize the object");

      // encrypt the archive.
      if (this->Encrypt(archive, code) == StatusError)
	escape("unable to encrypt the object's archive");

      leave();
    }

    Status		PublicKey::Encrypt(const Archivable&	o1,
					   const Archivable&	o2,
					   Code&		code) const
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2) == StatusError)
	escape("unable to serialize the object");

      if (this->Encrypt(archive, code) == StatusError)
	escape("unable to encrypt the object's archive");

      leave();
    }

    Status		PublicKey::Encrypt(const Archivable&	o1,
					   const Archivable&	o2,
					   const Archivable&	o3,
					   Code&		code) const
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3) == StatusError)
	escape("unable to serialize the object");

      if (this->Encrypt(archive, code) == StatusError)
	escape("unable to encrypt the object's archive");

      leave();
    }

    Status		PublicKey::Encrypt(const Archivable&	o1,
					   const Archivable&	o2,
					   const Archivable&	o3,
					   const Archivable&	o4,
					   Code&		code) const
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4) == StatusError)
	escape("unable to serialize the object");

      if (this->Encrypt(archive, code) == StatusError)
	escape("unable to encrypt the object's archive");

      leave();
    }

    Status		PublicKey::Encrypt(const Archivable&	o1,
					   const Archivable&	o2,
					   const Archivable&	o3,
					   const Archivable&	o4,
					   const Archivable&	o5,
					   Code&		code) const
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5) == StatusError)
	escape("unable to serialize the object");

      if (this->Encrypt(archive, code) == StatusError)
	escape("unable to encrypt the object's archive");

      leave();
    }

    Status		PublicKey::Encrypt(const Archivable&	o1,
					   const Archivable&	o2,
					   const Archivable&	o3,
					   const Archivable&	o4,
					   const Archivable&	o5,
					   const Archivable&	o6,
					   Code&		code) const
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5, o6) == StatusError)
	escape("unable to serialize the object");

      if (this->Encrypt(archive, code) == StatusError)
	escape("unable to encrypt the object's archive");

      leave();
    }

    Status		PublicKey::Encrypt(const Archivable&	o1,
					   const Archivable&	o2,
					   const Archivable&	o3,
					   const Archivable&	o4,
					   const Archivable&	o5,
					   const Archivable&	o6,
					   const Archivable&	o7,
					   Code&		code) const
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5, o6, o7) == StatusError)
	escape("unable to serialize the object");

      if (this->Encrypt(archive, code) == StatusError)
	escape("unable to encrypt the object's archive");

      leave();
    }

    Status		PublicKey::Encrypt(const Archivable&	o1,
					   const Archivable&	o2,
					   const Archivable&	o3,
					   const Archivable&	o4,
					   const Archivable&	o5,
					   const Archivable&	o6,
					   const Archivable&	o7,
					   const Archivable&	o8,
					   Code&		code) const
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5, o6, o7, o8) == StatusError)
	escape("unable to serialize the object");

      if (this->Encrypt(archive, code) == StatusError)
	escape("unable to encrypt the object's archive");

      leave();
    }

    Status		PublicKey::Encrypt(const Archivable&	o1,
					   const Archivable&	o2,
					   const Archivable&	o3,
					   const Archivable&	o4,
					   const Archivable&	o5,
					   const Archivable&	o6,
					   const Archivable&	o7,
					   const Archivable&	o8,
					   const Archivable&	o9,
					   Code&		code) const
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5, o6, o7, o8, o9) == StatusError)
	escape("unable to serialize the object");

      if (this->Encrypt(archive, code) == StatusError)
	escape("unable to encrypt the object's archive");

      leave();
    }

    ///
    /// verify
    ///

    ///
    /// this method verifies an Archivable object by serializing it
    /// before performing the verification process.
    ///
    Status		PublicKey::Verify(const Signature&	signature,
					  const Archivable&	o) const
    {
      Archive		archive;

      // create the archive.
      if (archive.Create() == StatusError)
	flee("unable to create the archive");

      // serialize the object.
      if (archive.Serialize(o) == StatusError)
	flee("unable to serialize the object");

      // call the Verify() method.
      if (this->Verify(signature, archive) != StatusTrue)
	flee("unable to verify the signature against the object's archive");

      true();
    }

    Status		PublicKey::Verify(const Signature&	signature,
					  const Archivable&	o1,
					  const Archivable&	o2) const
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	flee("unable to create the archive");

      if (archive.Serialize(o1, o2) == StatusError)
	flee("unable to serialize the objects");

      if (this->Verify(signature, archive) != StatusTrue)
	flee("unable to verify the signature against the objects' archive");

      true();
    }

    Status		PublicKey::Verify(const Signature&	signature,
					  const Archivable&	o1,
					  const Archivable&	o2,
					  const Archivable&	o3) const
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	flee("unable to create the archive");

      if (archive.Serialize(o1, o2, o3) == StatusError)
	flee("unable to serialize the objects");

      if (this->Verify(signature, archive) != StatusTrue)
	flee("unable to verify the signature against the objects' archive");

      true();
    }

    Status		PublicKey::Verify(const Signature&	signature,
					  const Archivable&	o1,
					  const Archivable&	o2,
					  const Archivable&	o3,
					  const Archivable&	o4) const
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	flee("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4) == StatusError)
	flee("unable to serialize the objects");

      if (this->Verify(signature, archive) != StatusTrue)
	flee("unable to verify the signature against the objects' archive");

      true();
    }

    Status		PublicKey::Verify(const Signature&	signature,
					  const Archivable&	o1,
					  const Archivable&	o2,
					  const Archivable&	o3,
					  const Archivable&	o4,
					  const Archivable&	o5) const
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	flee("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5) == StatusError)
	flee("unable to serialize the objects");

      if (this->Verify(signature, archive) != StatusTrue)
	flee("unable to verify the signature against the objects' archive");

      true();
    }

    Status		PublicKey::Verify(const Signature&	signature,
					  const Archivable&	o1,
					  const Archivable&	o2,
					  const Archivable&	o3,
					  const Archivable&	o4,
					  const Archivable&	o5,
					  const Archivable&	o6) const
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	flee("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5, o6) == StatusError)
	flee("unable to serialize the objects");

      if (this->Verify(signature, archive) != StatusTrue)
	flee("unable to verify the signature against the objects' archive");

      true();
    }

    Status		PublicKey::Verify(const Signature&	signature,
					  const Archivable&	o1,
					  const Archivable&	o2,
					  const Archivable&	o3,
					  const Archivable&	o4,
					  const Archivable&	o5,
					  const Archivable&	o6,
					  const Archivable&	o7) const
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	flee("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5, o6, o7) == StatusError)
	flee("unable to serialize the objects");

      if (this->Verify(signature, archive) != StatusTrue)
	flee("unable to verify the signature against the objects' archive");

      true();
    }

    Status		PublicKey::Verify(const Signature&	signature,
					  const Archivable&	o1,
					  const Archivable&	o2,
					  const Archivable&	o3,
					  const Archivable&	o4,
					  const Archivable&	o5,
					  const Archivable&	o6,
					  const Archivable&	o7,
					  const Archivable&	o8) const
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	flee("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5, o6, o7, o8) == StatusError)
	flee("unable to serialize the objects");

      if (this->Verify(signature, archive) != StatusTrue)
	flee("unable to verify the signature against the objects' archive");

      true();
    }

    Status		PublicKey::Verify(const Signature&	signature,
					  const Archivable&	o1,
					  const Archivable&	o2,
					  const Archivable&	o3,
					  const Archivable&	o4,
					  const Archivable&	o5,
					  const Archivable&	o6,
					  const Archivable&	o7,
					  const Archivable&	o8,
					  const Archivable&	o9) const
    {
      Archive		archive;

      if (archive.Create() == StatusError)
	flee("unable to create the archive");

      if (archive.Serialize(o1, o2, o3, o4, o5, o6, o7, o8, o9) == StatusError)
	flee("unable to serialize the objects");

      if (this->Verify(signature, archive) != StatusTrue)
	flee("unable to verify the signature against the objects' archive");

      true();
    }

  }
}
