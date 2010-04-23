//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/libraries/elle/cryptography/KeyPair.cc
//
// created       julien quintard   [sat oct 27 18:12:04 2007]
// updated       julien quintard   [thu apr 22 23:40:54 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/cryptography/KeyPair.hh>

namespace elle
{
  using namespace io;
  using namespace core;
  using namespace miscellaneous;
  using namespace archive;

  namespace cryptography
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

    ///
    /// this defines a null key pair.
    ///
    const KeyPair		KeyPair::Null;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes the key generation context.
    ///
    Status		KeyPair::Initialize()
    {
      enter();

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
      enter();

      // release the generation context.
      ::EVP_PKEY_CTX_free(KeyPair::Contexts::Generate);

      leave();
    }

    ///
    /// this method generate a RSA keypair.
    ///
    Status		KeyPair::Generate()
    {
      enter();

      if (this->Generate(KeyPair::Default::Length) == StatusError)
	escape("unable to generate the key pair");

      leave();
    }

    ///
    /// this method generates a key pair given the parameters.
    ///
    /// the argument length represents the length of the key, in bits.
    ///
    Status		KeyPair::Generate(const Natural32	length)
    {
      ::EVP_PKEY*	key;

      enter(slab(key, ::EVP_PKEY_free));

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

      // stop tracking.
      waive(key);

      leave();
    }

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// this method check if two keypairs match.
    ///
    Boolean		KeyPair::operator==(const KeyPair&	element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	true();

      // compare the internal keys.
      if ((this->K != element.K) || (this->k != element.k))
	false();

      true();
    }

    ///
    /// this macro-function call generates the entity.
    ///
    embed(Entity, KeyPair);

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the keypair internals.
    ///
    Status		KeyPair::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[KeyPair] " << *this << std::endl;

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
      enter();

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
      enter();

      // extract the internal keys.
      if (archive.Extract(this->K, this->k) == StatusError)
	escape("unable to extract the internal keys");

      leave();
    }

  }
}
