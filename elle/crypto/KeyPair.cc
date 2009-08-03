//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/crypto/KeyPair.cc
//
// created       julien quintard   [sat oct 27 18:12:04 2007]
// updated       julien quintard   [mon aug  3 20:58:18 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/crypto/KeyPair.hh>

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
    /// the default key pair length.
    ///
    const Natural32		KeyPair::Default::Length = 1024;

    ///
    /// the default value for the key generation context.
    ///
    ::EVP_PKEY_CTX*		KeyPair::Contexts::Generate = NULL;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes the key generation context.
    ///
    Status		KeyPair::Initialize()
    {
      // create the context for the RSA algorithm.
      if ((KeyPair::Contexts::Generate = ::EVP_PKEY_CTX_new_id(EVP_PKEY_RSA,
							       NULL)) == NULL)
	escape("unable to create the context");

      // initialise the context for key generation.
      if (::EVP_PKEY_keygen_init(KeyPair::Contexts::Generate) <= 0)
	escape("unable to initialise the generation context");

      leave();
    }

    ///
    /// this method cleans the key generation context.
    ///
    Status		KeyPair::Clean()
    {
      // release the generation context.
      ::EVP_PKEY_CTX_free(KeyPair::Contexts::Generate);
    }

    ///
    /// this method generate a RSA keypair.
    ///
    Status		KeyPair::Generate()
    {
      if (this->Generate(KeyPair::Default::Length) == StatusError)
	escape("unable to generate the key pair");

      leave();
    }

    ///
    /// this method generates a key pair given the parameters.
    ///
    Status		KeyPair::Generate(const Natural32	length)
    {
      ::EVP_PKEY*	key = NULL;

      // set the key length.
      if (::EVP_PKEY_CTX_set_rsa_keygen_bits(KeyPair::Contexts::Generate,
					     length) <= 0)
	escape("unable to set the RSA key length");

      // generate the EVP key.
      if (::EVP_PKEY_keygen(KeyPair::Contexts::Generate, &key) <= 0)
	escape("unable to generate the key");

      // create the actual public key according to the EVP structure.
      if (this->K.Create(key) == StatusError)
	escape("unable to create the public key");

      // create the actual private key according to the EVP structure.
      if (this->k.Create(key) == StatusError)
	escape("unable to create the private key");

      // release the memory.
      ::EVP_PKEY_free(key);

      leave();
    }

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// assign the keypair by duplicating the attributes.
    ///
    KeyPair&		KeyPair::operator=(const KeyPair&	element)
    {
      // self-check
      if (this == &element)
	return (*this);

      // recycle the keypair.
      if (this->Recycle<KeyPair>(&element) == StatusError)
	yield("unable to recycle the keypair", *this);

      return (*this);
    }

    ///
    /// this method check if two keypairs match.
    ///
    Boolean		KeyPair::operator==(const KeyPair&	element) const
    {
      // compare the internal keys.
      if ((this->K != element.K) || (this->k != element.k))
	false();

      true();
    }

    ///
    /// this method checks if two keypairs dis-match.
    ///
    Boolean		KeyPair::operator!=(const KeyPair&	element) const
    {
      return (!(*this == element));
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the keypair internals.
    ///
    Status		KeyPair::Dump(const Natural32		margin)
    {
      String		alignment(margin, ' ');

      std::cout << alignment << "[KeyPair]" << std::endl;

      if (this->K.Dump(margin + 2) == StatusError)
	escape("unable to dump the public key");

      if (this->k.Dump(margin + 2) == StatusError)
	escape("unable to dump the public key");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes a keypair object.
    ///
    Status		KeyPair::Serialize(Archive&		archive) const
    {
      // serialize the internal keys.
      if (archive.Serialize(this->K, this->k) == StatusError)
	escape("unable to serialize the internal keys");

      leave();
    }

    ///
    /// this method extract a keypair from the given archive.
    ///
    Status		KeyPair::Extract(Archive&		archive)
    {
      // extract the internal keys.
      if (archive.Extract(this->K, this->k) == StatusError)
	escape("unable to extract the internal keys");

      leave();
    }

  }
}
