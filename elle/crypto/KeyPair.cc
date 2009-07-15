//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       GPL
//
// file          /home/mycure/infinit/elle/crypto/KeyPair.cc
//
// created       julien quintard   [sat oct 27 18:12:04 2007]
// updated       julien quintard   [wed mar 11 16:19:03 2009]
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
    /// the class name.
    ///
    const String		KeyPair::Class = "KeyPair";

    ///
    /// the default key pair length.
    ///
    const Natural32		KeyPair::Default::Length = 2048;

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
    /// \todo
    ///   note that this context is never released :(
    ///
    Status		KeyPair::Initialize()
    {
      // if already initialized, leave.
      if (KeyPair::Contexts::Generate != NULL)
	leave();

      // create the context for the RSA algorithm.
      if ((KeyPair::Contexts::Generate = ::EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL)) == NULL)
	escape("unable to create the context");

      // initialise the context for key generation.
      if (::EVP_PKEY_keygen_init(KeyPair::Contexts::Generate) <= 0)
	escape("unable to initialise the generation context");

      leave();
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

      // initialize the generation context.
      if (KeyPair::Initialize() == StatusError)
	escape("unable to initialize the key generation context");

      // set the key length.
      if (::EVP_PKEY_CTX_set_rsa_keygen_bits(KeyPair::Contexts::Generate, length) <= 0)
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
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method initializes the object.
    ///
    Status		KeyPair::New(KeyPair&)
    {
      leave();
    }

    ///
    /// this method releases the resources.
    ///
    Status		KeyPair::Delete(KeyPair&)
    {
      leave();
    }

    ///
    /// assign the keypair by duplicating the attributes.
    ///
    KeyPair&		KeyPair::operator=(const KeyPair&		element)
    {
      // self-check
      if (this == &element)
	return (*this);

      // reinitialize the object.
      if ((KeyPair::Delete(*this) == StatusError) ||
	  (KeyPair::New(*this) == StatusError))
	yield("unable to reinitialize the object", *this);

      // just copy-assign the internal attributes.
      this->K = element.K;
      this->k = element.k;

      return (*this);
    }

    ///
    /// this method check if two keypairs match.
    ///
    Boolean		KeyPair::operator==(const KeyPair&	element)
    {
      // compare the internal keys.
      if ((this->K != element.K) || (this->k != element.k))
	false();

      true();
    }

    ///
    /// this method checks if two keypairs dis-match.
    ///
    Boolean		KeyPair::operator!=(const KeyPair&	element)
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
      Archive		ar;

      // prepare the object archive.
      if (ar.Create() == StatusError)
	escape("unable to prepare the object archive");

      // serialize the class name.
      if (ar.Serialize(KeyPair::Class) == StatusError)
	escape("unable to serialize the class name");

      // serialize the internal numbers.
      if (this->K.Serialize(ar) == StatusError)
	escape("unable to serialize the public key");

      if (this->k.Serialize(ar) == StatusError)
	escape("unable to serialize the private key");

      // record the object archive into the given archive.
      if (archive.Serialize(ar) == StatusError)
	escape("unable to serialize the object archive");

      leave();
    }

    ///
    /// this method extract a keypair from the given archive.
    ///
    Status		KeyPair::Extract(Archive&		archive)
    {
      Archive		ar;
      String		name;

      // extract the keypair archive object.
      if (archive.Extract(ar) == StatusError)
	escape("unable to extract the keypair archive object");

      // extract the name.
      if (ar.Extract(name) == StatusError)
	escape("unable to extract the class name from the archive");

      // check the name.
      if (KeyPair::Class != name)
	escape("unable to properly extract a keypair object from the archive");

      // extract the public key.
      if (this->K.Extract(ar) == StatusError)
	escape("unable to extract the public key from the keypair archive");

      // extract the private key.
      if (this->k.Extract(ar) == StatusError)
	escape("unable to extract the private key from the keypair archive");

      leave();
    }

  }
}

//
// ---------- operators -------------------------------------------------------
//

namespace std
{

  ///
  /// this function overloads the << operator and computes a fingerprint.
  ///
  std::ostream&		operator<<(std::ostream&		stream,
				   const elle::crypto::KeyPair&	key)
  {
    stream << "{";
    stream << key.K;
    stream << ", ";
    stream << key.k;
    stream << "}";

    return (stream);
  }

}
