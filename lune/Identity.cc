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
// updated       julien quintard   [thu may  6 01:12:08 2010]
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
  const elle::String		Identity::Extension = ".id";

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
  /// this method seals the identity with the authority.
  ///
  elle::Status		Identity::Seal(const Authority&		authority)
  {
    enter();

    // sign the pair with the authority.
    if (authority.k->Sign(this->name, this->pair,
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

    // verify the signature.
    if (authority.K.Verify(this->signature,
			   this->name, this->pair) != elle::StatusTrue)
      false();

    true();
  }

//
// ---------- object ----------------------------------------------------------
//

  ///
  /// this macro-function call generates the object.
  ///
  embed(Identity, _(elle::FormatBase64, elle::FormatCustom), _());

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

    // serialize the attributes.
    if (archive.Serialize(this->name,
			  this->pair,
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

    // extract the attributes.
    if (archive.Extract(this->name,
			this->pair,
			this->signature) == elle::StatusError)
      escape("unable to extract the attributes");

    leave();
  }

//
// ---------- fileable --------------------------------------------------------
//

  ///
  /// this method loads an identity.
  ///
  elle::Status		Identity::Load(const elle::String&	name,
				       const elle::String&	pass)
  {
    elle::String	path =
      Lune::Users +
      elle::System::Path::Separator +
      name +
      elle::System::Path::Separator +
      name +
      Identity::Extension;
    elle::Region	region;
    struct ::stat	status;
    elle::Cipher	cipher;
    elle::SecretKey	key;
    int			fd;

    enter();

    // retrieve information on the file, if it exsits.
    if (::stat(path.c_str(), &status) == -1)
      escape(::strerror(errno));

    // create a secret key with the given pass.
    if (key.Create(pass) == elle::StatusError)
      escape("unable to create the secret key");

    // prepare the region.
    if (region.Prepare(status.st_size) == elle::StatusError)
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
    if (elle::Base64::Decode(elle::String((char*)region.contents, region.size),
			     this->name, cipher, this->signature) ==
	elle::StatusError)
      escape("unable to decode the cipher");

    // decrypt the cipher file content with the secret key.
    if (key.Decrypt(cipher, this->pair) == elle::StatusError)
      escape("unable to decrypt the pair");

    leave();
  }

  ///
  /// this method stores a identity.
  ///
  elle::Status		Identity::Store(const elle::String&	name,
					const elle::String&	pass) const
  {
    elle::String	path =
      Lune::Users +
      elle::System::Path::Separator +
      name +
      elle::System::Path::Separator +
      name +
      Identity::Extension;
    elle::Cipher	cipher;
    elle::String	string;
    elle::SecretKey	key;
    int			fd;

    enter();

    // create a secret key with this pass.
    if (key.Create(pass) == elle::StatusError)
      escape("unable to create the secret key");

    // encrypt the pair.
    if (key.Encrypt(this->pair, cipher) == elle::StatusError)
      escape("unable to encrypt the pair");

    // encode in base64.
    if (elle::Base64::Encode(this->name, cipher, this->signature,
			     string) == elle::StatusError)
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

	escape("unable to write the identity file");
      }

    // close the file.
    ::close(fd);

    leave();
  }

}
