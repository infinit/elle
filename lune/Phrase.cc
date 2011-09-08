//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// file          /home/mycure/infinit/lune/Phrase.cc
//
// created       julien quintard   [tue may  4 20:49:38 2010]
// updated       julien quintard   [wed sep  7 21:36:00 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <lune/Phrase.hh>
#include <lune/Lune.hh>

namespace lune
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this string defines the phrase files extension.
  ///
  const elle::String		Phrase::Extension = ".phr";

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method creates a phrase.
  ///
  elle::Status		Phrase::Create(const elle::String&	string)
  {
    enter();

    // assign the string.
    this->string = string;

    leave();
  }

//
// ---------- object ----------------------------------------------------------
//

  ///
  /// this macro-function call generates the object.
  ///
  embed(Phrase, _());

//
// ---------- dumpable --------------------------------------------------------
//

  ///
  /// this method dumps the object.
  ///
  elle::Status		Phrase::Dump(const elle::Natural32	margin) const
  {
    elle::String	alignment(margin, ' ');

    enter();

    std::cout << alignment << "[Phrase] " << this->string << std::endl;

    leave();
  }

//
// ---------- archivable ------------------------------------------------------
//

  ///
  /// this method serializes the object.
  ///
  elle::Status		Phrase::Serialize(elle::Archive&	archive) const
  {
    enter();

    // serialize the attributes.
    if (archive.Serialize(this->string) == elle::StatusError)
      escape("unable to serialize the attributes");

    leave();
  }

  ///
  /// this method extracts the object.
  ///
  elle::Status		Phrase::Extract(elle::Archive&		archive)
  {
    enter();

    // extract the attributes.
    if (archive.Extract(this->string) == elle::StatusError)
      escape("unable to extract the attributes");

    leave();
  }

//
// ---------- fileable --------------------------------------------------------
//

  ///
  /// this method loads the user's local map.
  ///
  elle::Status		Phrase::Load(const elle::String&	user)
  {
    elle::Path		path;
    elle::Region	region;

    enter();

    // create the path.
    if (path.Create(Lune::User::Phrase) == elle::StatusError)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::Piece("%USER%", user)) == elle::StatusError)
      escape("unable to complete the path");

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
  /// this method stores the user's local map.
  ///
  elle::Status		Phrase::Store(const elle::String&	user) const
  {
    elle::Path		path;
    elle::Region	region;
    elle::String	string;

    enter();

    // create the path.
    if (path.Create(Lune::User::Phrase) == elle::StatusError)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::Piece("%USER%", user)) == elle::StatusError)
      escape("unable to complete the path");

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
  /// this method erases the phrase.
  ///
  elle::Status		Phrase::Erase(const elle::String&	user) const
  {
    elle::Path		path;

    enter();

    // create the path.
    if (path.Create(Lune::User::Phrase) == elle::StatusError)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::Piece("%USER%", user)) == elle::StatusError)
      escape("unable to complete the path");

    // erase the file.
    if (elle::File::Erase(path) == elle::StatusError)
      escape("unable to erase the file");

    leave();
  }

  ///
  /// this method tests the phrase.
  ///
  elle::Status		Phrase::Exist(const elle::String&	user) const
  {
    elle::Path		path;

    enter();

    // create the path.
    if (path.Create(Lune::User::Phrase) == elle::StatusError)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::Piece("%USER%", user)) == elle::StatusError)
      escape("unable to complete the path");

    // test the file.
    if (elle::File::Exist(path) == elle::StatusFalse)
      false();

    true();
  }

}
