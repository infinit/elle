//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/cryptography/KeyPair.cc
//
// created       julien quintard   [sat oct 27 18:12:04 2007]
// updated       julien quintard   [wed may  5 13:12:41 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/cryptography/KeyPair.hh>
#include <elle/cryptography/Cipher.hh>
#include <elle/cryptography/SecretKey.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

#include <elle/idiom/Close.hh>
# include <openssl/engine.h>
# include <openssl/err.h>
# include <openssl/evp.h>
# include <fcntl.h>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace standalone;

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

    ///
    /// this string defines the key pair files extension.
    ///
    const String		KeyPair::Extension = ".pair";

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

    ///
    /// this method creates a key pair from both a public and private key.
    ///
    Status		KeyPair::Create(const PublicKey&	K,
					const PrivateKey&	k)
    {
      enter();

      // assign the attributes.
      this->K = K;
      this->k = k;

      leave();
    }

//
// ---------- object ----------------------------------------------------------
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
    embed(KeyPair, _(FormatBase64, FormatCustom), _());

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

      std::cout << alignment << "[KeyPair]" << std::endl;

      // dump the public key.
      if (this->K.Dump(margin + 2) == StatusError)
	escape("unable to dump the public key");

      // dump the private key.
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

//
// ---------- fileable --------------------------------------------------------
//

    ///
    /// this method loads a key pair from a file.
    ///
    Status		KeyPair::Load(const String&		name,
				      const String&		pass)
    {
      String		path = name + KeyPair::Extension;
      Region		region;
      struct ::stat	status;
      Cipher		cipher;
      SecretKey		key;
      int		fd;

      enter();

      // retrieve information on the file, if it exsits.
      if (::stat(path.c_str(), &status) == -1)
	escape(::strerror(errno));

      // create a secret key with the given pass.
      if (key.Create(pass) == StatusError)
	escape("unable to create the secret key");

      // prepare the region.
      if (region.Prepare(status.st_size) == StatusError)
	escape("unable to prepare the region");

      // set the correct size.
      region.size = status.st_size;

      // open the file.
      if ((fd = ::open(path.c_str(), O_RDONLY)) == -1)
	escape(::strerror(errno));

      // read the file's content.
      if (::read(fd, region.contents, region.size) == -1)
	escape(::strerror(errno));

      // close the file.
      ::close(fd);

      // decode and extract the cipher.
      if (Base64::Decode(String((char*)region.contents, region.size),
			 cipher) == StatusError)
	escape("unable to decode the cipher");

      // decrypt the cipher file content with the secret key.
      if (key.Decrypt(cipher, *this) == StatusError)
	escape("unable to decrypt the keypair");

      leave();
    }

    ///
    /// this method stores a key pair in a file, taking care to encrypt
    /// it with the given pass.
    ///
    Status		KeyPair::Store(const String&		name,
				       const String&		pass) const
    {
      String		path = name + KeyPair::Extension;
      struct ::stat	status;
      Cipher		cipher;
      String		string;
      SecretKey		key;
      int		fd;

      enter();

      // create a secret key with this pass.
      if (key.Create(pass) == StatusError)
	escape("unable to create the secret key");

      // encrypt the keypair.
      if (key.Encrypt(*this, cipher) == StatusError)
	escape("unable to decrypt the keypair");

      // encode in base64.
      if (Base64::Encode(cipher, string) == StatusError)
	escape("unable to encode in base64");

      // open the file.
      if ((fd = ::open(path.c_str(),
		       O_WRONLY | O_CREAT | O_TRUNC,
		       0400)) == -1)
	escape(::strerror(errno));

      // write the file.
      if (::write(fd, string.c_str(), string.length()) != string.length())
	{
	  ::close(fd);

	  escape("unable to write the key pair file");
	}

      // close the file.
      ::close(fd);

      leave();
    }

  }
}
