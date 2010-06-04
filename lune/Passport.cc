//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// file          /home/mycure/infinit/lune/Passport.cc
//
// created       julien quintard   [sat may  1 21:19:13 2010]
// updated       julien quintard   [fri may 28 18:14:50 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <lune/Passport.hh>
#include <lune/Lune.hh>

namespace lune
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this string defines the passport files extension.
  ///
  const elle::String		Passport::Extension = ".ppt";

//
// ---------- constructors & destructors --------------------------------------
//

  ///
  /// default constructor.
  ///
  Passport::Passport():
    pair(NULL),
    code(NULL)
  {
  }

  ///
  /// destructor.
  ///
  Passport::~Passport()
  {
    // release the pair.
    if (this->pair != NULL)
      delete this->pair;

    // release the code.
    if (this->code != NULL)
      delete this->code;
  }

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method creates a passport.
  ///
  elle::Status		Passport::Create(const elle::String&	user,
					 const elle::String&	universe)
  {
    enter();

    // set the attributes.
    this->user = user;
    this->universe = universe;

    // allocate a pair.
    this->pair = new elle::KeyPair;

    // generate a key pair.
    if (this->pair->Generate() == elle::StatusError)
      escape("unable to generate the passport's key pair");

    leave();
  }

  ///
  /// this method encrypts the passport.
  ///
  elle::Status		Passport::Encrypt(const lune::Identity&	identity)
  {
    enter();

    // check the pair.
    if (this->pair == NULL)
      escape("unable to encrypt an unexisting key pair");

    // allocate the code
    this->code = new elle::Code;

    // encrypt the key pair.
    if (identity.pair.K.Encrypt(*this->pair,
				*this->code) == elle::StatusError)
      escape("unable to encrypt the passport's pair");

    leave();
  }

  ///
  /// this method decrypts the passport.
  ///
  elle::Status		Passport::Decrypt(const lune::Identity&	identity)
  {
    enter();

    // check the code.
    if (this->code == NULL)
      escape("unable to decrypt an unencrypted passport");

    // allocate the key pair.
    this->pair = new elle::KeyPair;

    // decrypt the key pair.
    if (identity.pair.k.Decrypt(*this->code,
				*this->pair) == elle::StatusError)
      escape("unable to decrypt the passport's pair");

    leave();
  }

  ///
  /// this method seals the passport with the authority.
  ///
  elle::Status		Passport::Seal(const Authority&		authority)
  {
    enter();

    // check the code.
    if (this->code == NULL)
      escape("unable to seal an unencrypted passport");

    // sign the attributesr with the authority.
    if (authority.k->Sign(this->user, this->universe, *this->code,
			  this->signature) == elle::StatusError)
      escape("unable to sign the attributes with the authority");

    leave();
  }

  ///
  /// this method verifies the validity of the passport.
  ///
  elle::Status		Passport::Validate(const Authority&	authority)
    const
  {
    enter();

    // check the code.
    if (this->code == NULL)
      escape("unable to validate an unencrypted passport");

    // verify the signature.
    if (authority.K.Verify(this->signature,
			   this->user, this->universe, *this->code) !=
	elle::StatusTrue)
      false();

    true();
  }

  ///
  /// this method loads a user passport.
  ///
  elle::Status		Passport::Load(const elle::String&	universe)
  {
    elle::String	path =
      Lune::User::Passports + elle::System::Path::Separator +
      Lune::User::Name + elle::String("::") + universe + Passport::Extension;
    elle::Region	region;

    enter();

    // check the mode.
    if (Lune::Environment != Lune::ModeUser)
      escape("unable to manipulate passport files in this mode");

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
  /// this method stores a passport.
  ///
  elle::Status		Passport::Store(const elle::String&	universe) const
  {
    elle::String	path =
      Lune::User::Passports + elle::System::Path::Separator +
      Lune::User::Name + elle::String("::") + universe + Passport::Extension;
    elle::Region	region;
    elle::String	string;

    enter();

    // check the mode.
    if (Lune::Environment != Lune::ModeUser)
      escape("unable to manipulate passport files in this mode");

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
  /// this method loads a system passport.
  ///
  elle::Status		Passport::Load(const elle::String&	user,
				       const elle::String&	universe)
  {
    elle::String	path =
      Lune::System::Passports + elle::System::Path::Separator +
      user + elle::String("::") + universe + Passport::Extension;
    elle::Region	region;

    enter();

    // check the mode.
    if (Lune::Environment != Lune::ModeSystem)
      escape("unable to manipulate passport files in this mode");

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
  /// this method stores a system passport.
  ///
  elle::Status		Passport::Store(const elle::String&	user,
					const elle::String&	universe) const
  {
    elle::String	path =
      Lune::System::Passports + elle::System::Path::Separator +
      user + elle::String("::") + universe + Passport::Extension;
    elle::Region	region;
    elle::String	string;

    enter();

    // check the mode.
    if (Lune::Environment != Lune::ModeSystem)
      escape("unable to manipulate passport files in this mode");

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
  embed(Passport, _());

//
// ---------- dumpable --------------------------------------------------------
//

  ///
  /// this method dumps a passport.
  ///
  elle::Status		Passport::Dump(const elle::Natural32	margin) const
  {
    elle::String	alignment(margin, ' ');

    enter();

    std::cout << alignment << "[Passport] " << std::endl;

    // dump the user.
    std::cout << alignment << elle::Dumpable::Shift
	      << "[User] " << this->user << std::endl;

    // dump the universe.
    std::cout << alignment << elle::Dumpable::Shift
	      << "[Universe] " << this->universe << std::endl;

    // dump the pair.
    if (this->pair != NULL)
      {
	if (this->pair->Dump(margin + 2) == elle::StatusError)
	  escape("unable to dump the pair");
      }

    // dump the code.
    if (this->code != NULL)
      {
	if (this->code->Dump(margin + 2) == elle::StatusError)
	  escape("unable to dump the code");
      }

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
  elle::Status		Passport::Serialize(elle::Archive&	archive) const
  {
    enter();

    // check the code.
    if (this->code == NULL)
      escape("unable to serialize an unencrypted passport");

    // serialize the attributes.
    if (archive.Serialize(this->user,
			  this->universe,
			  *this->code,
			  this->signature) == elle::StatusError)
      escape("unable to serialize the attributes");

    leave();
  }

  ///
  /// this method extracts the object.
  ///
  elle::Status		Passport::Extract(elle::Archive&	archive)
  {
    enter();

    // allocate the code.
    this->code = new elle::Code;

    // extract the attributes.
    if (archive.Extract(this->user,
			this->universe,
			*this->code,
			this->signature) == elle::StatusError)
      escape("unable to extract the attributes");

    leave();
  }

}
