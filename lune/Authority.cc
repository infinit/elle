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
// updated       julien quintard   [wed may  5 21:38:30 2010]
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
    k(NULL)
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

    // set the public key.
    this->K = K;

    leave();
  }

//
// ---------- object ----------------------------------------------------------
//

  ///
  /// this macro-function call generates the object.
  ///
  embed(Authority, _(elle::FormatBase64, elle::FormatCustom), _());

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

    // retrive the public key's unique.
    if (this->K.Save(unique) == elle::StatusError)
      escape("unable to save the public key");

    // dump the public key's unique so that it can be easily hard-coded in the
    // infinit software sources.
    std::cout << alignment << elle::Dumpable::Shift
	      << "[Unique] " << unique << std::endl;

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

    // serialize the public key.
    if (archive.Serialize(this->K) == elle::StatusError)
      escape("unable to serialize the public key");

    // serialize the private key.
    if (this->k != NULL)
      {
	// serialize the key.
	if (archive.Serialize(*this->k) == elle::StatusError)
	  escape("unable to serialize the private key");
      }
    else
      {
	// serialize 'none'.
	if (archive.Serialize(elle::none) == elle::StatusError)
	  escape("unable to serialize 'none'");
      }

    leave();
  }

  ///
  /// this method extracts the object.
  ///
  elle::Status		Authority::Extract(elle::Archive&	archive)
  {
    elle::Archive::Type	type;

    enter();

    // extract the public key.
    if (archive.Extract(this->K) == elle::StatusError)
      escape("unable to extract the attributes");

    // fetch the next element's type.
    if (archive.Fetch(type) == elle::StatusError)
      escape("unable to fetch the next element's type");

    // extract the private key.
    if (type == elle::Archive::TypeNull)
      {
	// nothing to extract.
	if (archive.Extract(elle::none) == elle::StatusError)
	  escape("unable to extract null");
      }
    else
      {
	// allocate a private key.
	this->k = new elle::PrivateKey;

	// extract the internal digest.
	if (archive.Extract(*this->k) == elle::StatusError)
	  escape("unable to extract the private key");
	}

    leave();
  }

//
// ---------- fileable --------------------------------------------------------
//

  ///
  /// this method loads an authority.
  ///
  elle::Status		Authority::Load(const elle::String&	pass)
  {
    elle::String	path =
      Lune::Authority + Authority::Extension;
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
			     cipher) == elle::StatusError)
      escape("unable to decode the cipher");

    // decrypt the cipher file content with the secret key.
    if (key.Decrypt(cipher, *this) == elle::StatusError)
      escape("unable to decrypt the authority");

    leave();
  }

  ///
  /// this method stores a authority.
  ///
  elle::Status		Authority::Store(const elle::String&	pass) const
  {
    elle::String	path =
      Lune::Authority + Authority::Extension;
    elle::Cipher	cipher;
    elle::String	string;
    elle::SecretKey	key;
    int			fd;

    enter();

    // create a secret key with this pass.
    if (key.Create(pass) == elle::StatusError)
      escape("unable to create the secret key");

    // encrypt the authority.
    if (key.Encrypt(*this, cipher) == elle::StatusError)
      escape("unable to encrypt the authority");

    // encode in base64.
    if (elle::Base64::Encode(cipher, string) == elle::StatusError)
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

	escape("unable to write the authority file");
      }

    // close the file.
    ::close(fd);

    leave();
  }

}
