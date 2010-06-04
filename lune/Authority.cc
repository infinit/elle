//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// file          /home/mycure/infinit/lune/Authority.cc
//
// created       julien quintard   [tue may  4 23:47:55 2010]
// updated       julien quintard   [fri may 28 18:05:53 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <lune/Authority.hh>
#include <lune/Lune.hh>

namespace lune
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this string defines the authority files extension.
  ///
  const elle::String		Authority::Extension = ".auth";

//
// ---------- constructors & destructors --------------------------------------
//

  ///
  /// default constructor.
  ///
  Authority::Authority():
    k(NULL),
    cipher(NULL)
  {
  }

  ///
  /// destructor.
  ///
  Authority::~Authority()
  {
    // release the private key, if present.
    if (this->k != NULL)
      delete this->k;

    // release the cipher.
    if (this->cipher != NULL)
      delete this->cipher;
  }

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method creates an authority based on the given key pair.
  ///
  elle::Status		Authority::Create(const elle::KeyPair&	pair)
  {
    enter();

    // set the type.
    this->type = Authority::TypePair;

    // set the public key.
    this->K = pair.K;

    // allocate the private key.
    this->k = new elle::PrivateKey(pair.k);

    leave();
  }

  ///
  /// this method creates an authority based on the given public key only.
  ///
  elle::Status		Authority::Create(const elle::PublicKey&	K)
  {
    enter();

    // set the type.
    this->type = Authority::TypePublic;

    // set the public key.
    this->K = K;

    leave();
  }

  ///
  /// this method encrypts the keys.
  ///
  elle::Status		Authority::Encrypt(const elle::String&		pass)
  {
    elle::SecretKey	key;

    enter();

    // create a secret key with this pass.
    if (key.Create(pass) == elle::StatusError)
      escape("unable to create the secret key");

    // allocate the cipher.
    this->cipher = new elle::Cipher;

    // encrypt depending on the type.
    switch (this->type)
      {
      case Authority::TypePair:
	{
	  // encrypt the authority.
	  if (key.Encrypt(this->K, *this->k,
			  *this->cipher) == elle::StatusError)
	    escape("unable to encrypt the authority");

	  break;
	}
      case Authority::TypePublic:
	{
	  // encrypt the authority.
	  if (key.Encrypt(this->K,
			  *this->cipher) == elle::StatusError)
	    escape("unable to encrypt the authority");

	  break;
	}
      case Authority::TypeUnknown:
      default:
	{
	  escape("unknown type");
	}
      }

    leave();
  }

  ///
  /// this method decrypts the keys.
  ///
  elle::Status		Authority::Decrypt(const elle::String&		pass)
  {
    elle::SecretKey	key;

    enter();

    // check the cipher.
    if (this->cipher == NULL)
      escape("unable to decrypt an unencrypted authority");

    // create a secret key with this pass.
    if (key.Create(pass) == elle::StatusError)
      escape("unable to create the secret key");

    // decrypt depending on the type.
    switch (this->type)
      {
      case Authority::TypePair:
	{
	  // allocate the private key.
	  this->k = new elle::PrivateKey;

	  // decrypt the authority.
	  if (key.Decrypt(*this->cipher,
			  this->K, *this->k) == elle::StatusError)
	    escape("unable to decrypt the authority");

	  break;
	}
      case Authority::TypePublic:
	{
	  // decrypt the authority.
	  if (key.Decrypt(*this->cipher,
			  this->K) == elle::StatusError)
	    escape("unable to decrypt the authority");

	  break;
	}
      case Authority::TypeUnknown:
      default:
	{
	  escape("unknown type");
	}
      }

    leave();
  }

  ///
  /// this method loads the system's authority file.
  ///
  elle::Status		Authority::Load()
  {
    elle::String	path =
      Lune::System::Authority + Authority::Extension;
    elle::Region	region;

    enter();

    // check the mode.
    if (Lune::Environment != Lune::ModeSystem)
      escape("unable to manipulate authority files in this mode");

    // read the file's content.
    if (elle::File::Read(path, region) == elle::StatusError)
      escape("unable to read the file's content");

    // decode and extract the object.
    if (elle::Hexadecimal::Decode(elle::String((char*)region.contents,
					       region.size),
				  *this) == elle::StatusError)
      escape("unable to decode the object");

    leave();
  }

  ///
  /// this method stores a authority.
  ///
  elle::Status		Authority::Store() const
  {
    elle::String	path =
      Lune::System::Authority + Authority::Extension;
    elle::Region	region;
    elle::String	string;

    enter();

    // check the mode.
    if (Lune::Environment != Lune::ModeSystem)
      escape("unable to manipulate authority files in this mode");

    // encode in hexadecimal.
    if (elle::Hexadecimal::Encode(*this, string) == elle::StatusError)
      escape("unable to encode the object in hexadecimal");

    // wrap the string.
    if (region.Wrap((elle::Byte*)string.c_str(),
		    string.length()) == elle::StatusError)
      escape("unable to wrap the string in a region");

    // write the file's content.
    if (elle::File::Write(path, region) == elle::StatusError)
      escape("unable to write the file's content");

    leave();
  }

//
// ---------- object ----------------------------------------------------------
//

  ///
  /// this macro-function call generates the object.
  ///
  embed(Authority, _());

//
// ---------- dumpable --------------------------------------------------------
//

  ///
  /// this method dumps a authority.
  ///
  elle::Status		Authority::Dump(const elle::Natural32	margin) const
  {
    elle::String	alignment(margin, ' ');
    elle::String	unique;

    enter();

    std::cout << alignment << "[Authority]" << std::endl;

    // dump the type.
    std::cout << alignment << elle::Dumpable::Shift
	      << "[Type] " << (elle::Natural32)this->type << std::endl;

    // dump the public key.
    if (this->K.Dump(margin + 2) == elle::StatusError)
      escape("unable to dump the public key");

    // if present...
    if (this->k != NULL)
      {
	// ...dump the private key.
	if (this->k->Dump(margin + 2) == elle::StatusError)
	  escape("unable to dump the private key");
      }

    // dump the cipher.
    if (this->cipher != NULL)
      {
	if (this->cipher->Dump(margin + 2) == elle::StatusError)
	  escape("unable to dump the cipher");
      }

    leave();
  }

//
// ---------- archivable ------------------------------------------------------
//

  ///
  /// this method serializes the object.
  ///
  elle::Status		Authority::Serialize(elle::Archive&	archive) const
  {
    enter();

    // check the cipher.
    if (this->cipher == NULL)
      escape("unable to serialize an unencrypted authority");

    // serialize the type and cipher.
    if (archive.Serialize((elle::Byte&)this->type,
			  *this->cipher) == elle::StatusError)
      escape("unable to serialize the attributes");

    leave();
  }

  ///
  /// this method extracts the object.
  ///
  elle::Status		Authority::Extract(elle::Archive&	archive)
  {
    elle::Byte		type;

    enter();

    // allocate the cipher.
    this->cipher = new elle::Cipher;

    // extract the type and cipher.
    if (archive.Extract(type, *this->cipher) == elle::StatusError)
      escape("unable to extract the attributes");

    // set the type.
    this->type = (Authority::Type)type;

    leave();
  }

}
