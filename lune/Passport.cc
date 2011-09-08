//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// file          /home/mycure/infinit/lune/Passport.cc
//
// created       julien quintard   [thu aug 11 13:31:24 2011]
// updated       julien quintard   [wed sep  7 21:35:35 2011]
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
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method creates a passport.
  ///
  elle::Status		Passport::Create(const hole::Label&	label)
  {
    enter();

    // set the attributes.
    this->label = label;

    leave();
  }

  ///
  /// this method seals the passport with the authority.
  ///
  elle::Status		Passport::Seal(const Authority&		authority)
  {
    enter();

    // sign the pair with the authority.
    if (authority.k->Sign(this->label,
			  this->signature) == elle::StatusError)
      escape("unable to sign the pair with the authority");

    leave();
  }

  ///
  /// this method verifies the validity of the passport.
  ///
  elle::Status		Passport::Validate(const Authority&	authority)
    const
  {
    enter();

    // verify the signature.
    if (authority.K.Verify(this->signature,
			   this->label) == elle::StatusError)
      escape("unable to verify the signature");

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

    std::cout << alignment << "[Passport]" << std::endl;

    // dump the label.
    if (this->label.Dump(margin + 2) == elle::StatusError)
      escape("unable to dump the label");

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

    // serialize the attributes.
    if (archive.Serialize(this->label,
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

    // extract the attributes.
    if (archive.Extract(this->label,
			this->signature) == elle::StatusError)
      escape("unable to extract the attributes");

    leave();
  }

//
// ---------- fileable --------------------------------------------------------
//

  ///
  /// this method loads a system named passport file.
  ///
  elle::Status		Passport::Load()
  {
    elle::Path		path;
    elle::Region	region;

    enter();

    // create the path.
    if (path.Create(Lune::Passport) == elle::StatusError)
      escape("unable to create the path");

    // read the file's content.
    if (elle::File::Read(path, region) == elle::StatusError)
      escape("unable to read the file's content");

    // decode and extract the object.
    if (elle::Hexadecimal::Decode(
	  elle::String(reinterpret_cast<char*>(region.contents),
		       region.size),
	  *this) == elle::StatusError)
      escape("unable to decode the object");

    leave();
  }

  ///
  /// this method stores a system named user passport.
  ///
  elle::Status		Passport::Store() const
  {
    elle::Path		path;
    elle::Region	region;
    elle::String	string;

    enter();

    // create the path.
    if (path.Create(Lune::Passport) == elle::StatusError)
      escape("unable to create the path");

    // encode in hexadecimal.
    if (elle::Hexadecimal::Encode(*this, string) == elle::StatusError)
      escape("unable to encode the object in hexadecimal");

    // wrap the string.
    if (region.Wrap(reinterpret_cast<const elle::Byte*>(string.c_str()),
		    string.length()) == elle::StatusError)
      escape("unable to wrap the string in a region");

    // write the file's content.
    if (elle::File::Write(path, region) == elle::StatusError)
      escape("unable to write the file's content");

    leave();
  }

  ///
  /// this method erases the passport.
  ///
  elle::Status		Passport::Erase() const
  {
    elle::Path		path;

    enter();

    // create the path.
    if (path.Create(Lune::Passport) == elle::StatusError)
      escape("unable to create the path");

    // erase the file.
    if (elle::File::Erase(path) == elle::StatusError)
      escape("unable to erase the file");

    leave();
  }

  ///
  /// this method tests the passport.
  ///
  elle::Status		Passport::Exist() const
  {
    elle::Path		path;

    enter();

    // create the path.
    if (path.Create(Lune::Passport) == elle::StatusError)
      escape("unable to create the path");

    // test the file.
    if (elle::File::Exist(path) == elle::StatusFalse)
      false();

    true();
  }

}
