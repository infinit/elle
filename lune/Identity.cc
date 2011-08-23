//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// file          /home/mycure/infinit/lune/Identity.cc
//
// created       julien quintard   [tue may  4 23:47:55 2010]
// updated       julien quintard   [thu aug 11 13:31:20 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <lune/Identity.hh>
#include <lune/Lune.hh>

namespace lune
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this string defines the identity files extension.
  ///
  const elle::String		Identity::Extension = ".idy";

//
// ---------- constructors & destructors --------------------------------------
//

  ///
  /// default constructor.
  ///
  Identity::Identity():
    cipher(NULL)
  {
  }

  ///
  /// destructor.
  ///
  Identity::~Identity()
  {
    // release the cipher.
    if (this->cipher != NULL)
      delete this->cipher;
  }

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method creates an identity based on the given key pair.
  ///
  elle::Status		Identity::Create(const elle::String&	name,
					 const elle::KeyPair&	pair)
  {
    enter();

    // set the name.
    this->name = name;

    // set the key pair.
    this->pair = pair;

    leave();
  }

  ///
  /// this method encrypts the key pair.
  ///
  elle::Status		Identity::Encrypt(const elle::String&	pass)
  {
    elle::SecretKey	key;

    enter();

    // create a secret key with this pass.
    if (key.Create(pass) == elle::StatusError)
      escape("unable to create the secret key");

    // allocate the cipher.
    this->cipher = new elle::Cipher;

    // encrypt the authority.
    if (key.Encrypt(this->pair,
		    *this->cipher) == elle::StatusError)
      escape("unable to encrypt the key pair");

    leave();
  }

  ///
  /// this method decrypts the key pair.
  ///
  elle::Status		Identity::Decrypt(const elle::String&	pass)
  {
    elle::SecretKey	key;

    enter();

    // check the cipher.
    if (this->cipher == NULL)
      escape("unable to decrypt an unencrypted identity");

    // create a secret key with this pass.
    if (key.Create(pass) == elle::StatusError)
      escape("unable to create the secret key");

    // decrypt the authority.
    if (key.Decrypt(*this->cipher,
		    this->pair) == elle::StatusError)
      escape("unable to decrypt the key pair");

    leave();
  }

  ///
  /// this method seals the identity with the authority.
  ///
  elle::Status		Identity::Seal(const Authority&		authority)
  {
    enter();

    // check the cipher.
    if (this->cipher == NULL)
      escape("unable to seal an unencrypted identity");

    // sign the pair with the authority.
    if (authority.k->Sign(this->name, *this->cipher,
			  this->signature) == elle::StatusError)
      escape("unable to sign the pair with the authority");

    leave();
  }

  ///
  /// this method verifies the validity of the identity.
  ///
  elle::Status		Identity::Validate(const Authority&	authority)
    const
  {
    enter();

    // check the cipher.
    if (this->cipher == NULL)
      escape("unable to verify an unencrypted identity");

    // verify the signature.
    if (authority.K.Verify(this->signature,
			   this->name, *this->cipher) == elle::StatusError)
      escape("unable to verify the signature");

    leave();
  }

//
// ---------- object ----------------------------------------------------------
//

  ///
  /// this macro-function call generates the object.
  ///
  embed(Identity, _());

//
// ---------- dumpable --------------------------------------------------------
//

  ///
  /// this method dumps a identity.
  ///
  elle::Status		Identity::Dump(const elle::Natural32	margin) const
  {
    elle::String	alignment(margin, ' ');

    enter();

    std::cout << alignment << "[Identity]" << std::endl;

    // dump the name.
    std::cout << alignment << elle::Dumpable::Shift
	      << "[Name] " << this->name << std::endl;

    // dump the pair.
    if (this->pair.Dump(margin + 2) == elle::StatusError)
      escape("unable to dump the pair");

    // dump the signature.
    if (this->signature.Dump(margin + 2) == elle::StatusError)
      escape("unable to dump the signature");

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
  elle::Status		Identity::Serialize(elle::Archive&	archive) const
  {
    enter();

    // check the cipher.
    if (this->cipher == NULL)
      escape("unable to serialize an unencrypted identity");

    // serialize the attributes.
    if (archive.Serialize(this->name,
			  *this->cipher,
			  this->signature) == elle::StatusError)
      escape("unable to serialize the attributes");

    leave();
  }

  ///
  /// this method extracts the object.
  ///
  elle::Status		Identity::Extract(elle::Archive&	archive)
  {
    enter();

    // allocate the cipher.
    this->cipher = new elle::Cipher;

    // extract the attributes.
    if (archive.Extract(this->name,
			*this->cipher,
			this->signature) == elle::StatusError)
      escape("unable to extract the attributes");

    leave();
  }

//
// ---------- fileable --------------------------------------------------------
//

  ///
  /// this method loads a system named identity file.
  ///
  elle::Status		Identity::Load(const elle::String&	name)
  {
    elle::Path		path;
    elle::Region	region;

    enter();

    // create the path.
    if (path.Create(Lune::User::Identity) == elle::StatusError)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::Piece("%USER%", name)) == elle::StatusError)
      escape("unable to complete the path");

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
  /// this method stores a system named user identity.
  ///
  elle::Status		Identity::Store(const elle::String&	name) const
  {
    elle::Path		path;
    elle::Region	region;
    elle::String	string;

    enter();

    // create the path.
    if (path.Create(Lune::User::Identity) == elle::StatusError)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::Piece("%USER%", name)) == elle::StatusError)
      escape("unable to complete the path");

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

  ///
  /// this method erases the identity.
  ///
  elle::Status		Identity::Erase(const elle::String&	name) const
  {
    elle::Path		path;

    enter();

    // create the path.
    if (path.Create(Lune::User::Identity) == elle::StatusError)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::Piece("%USER%", name)) == elle::StatusError)
      escape("unable to complete the path");

    // erase the file.
    if (elle::File::Erase(path) == elle::StatusError)
      escape("unable to erase the file");

    leave();
  }

  ///
  /// this method tests the identity.
  ///
  elle::Status		Identity::Exist(const elle::String&	name) const
  {
    elle::Path		path;

    enter();

    // create the path.
    if (path.Create(Lune::User::Identity) == elle::StatusError)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::Piece("%USER%", name)) == elle::StatusError)
      escape("unable to complete the path");

    // test the file.
    if (elle::File::Exist(path) == elle::StatusFalse)
      false();

    true();
  }

}
