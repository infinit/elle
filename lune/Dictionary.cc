//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       dictionary
//
// author        julien quintard   [mon may 10 16:14:02 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <lune/Dictionary.hh>
#include <lune/Lune.hh>

namespace lune
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this string defines the dictionary files extension.
  ///
  const elle::String            Dictionary::Extension = ".dic";

//
// ---------- object ----------------------------------------------------------
//

  ///
  /// this macro-function call generates the object.
  ///
  embed(Dictionary, _());

//
// ---------- dumpable --------------------------------------------------------
//

  ///
  /// this method dumps the dictionary.
  ///
  elle::Status          Dictionary::Dump(const elle::Natural32  margin) const
  {
    elle::String        alignment(margin, ' ');

    enter();

    std::cout << alignment << "[Dictionary]" << std::endl;

    // dump the users map.
    if (this->users.Dump(margin + 2) == elle::StatusError)
      escape("unable to dump the users map");

    // dump the groups map.
    if (this->groups.Dump(margin + 2) == elle::StatusError)
      escape("unable to dump the groups map");

    leave();
  }

//
// ---------- archivable ------------------------------------------------------
//

  ///
  /// this method serializes a dictionary.
  ///
  elle::Status          Dictionary::Serialize(elle::Archive&    archive) const
  {
    enter();

    // serialize the maps.
    if (archive.Serialize(this->users, this->groups) == elle::StatusError)
      escape("unable to serialize the maps");

    leave();
  }

  ///
  /// this method extract a public key from the given archive.
  ///
  elle::Status          Dictionary::Extract(elle::Archive&      archive)
  {
    enter();

    // extract the maps.
    if (archive.Extract(this->users, this->groups) == elle::StatusError)
      escape("unable to extract the maps");

    leave();
  }

//
// ---------- fileable --------------------------------------------------------
//

  ///
  /// this method loads the current user's local dictionary.
  ///
  elle::Status          Dictionary::Load()
  {
    elle::Path          path;
    elle::Region        region;

    enter();

    // create the path.
    if (path.Create(Lune::Dictionary) == elle::StatusError)
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
  /// this method stores the current user's local dictionary.
  ///
  elle::Status          Dictionary::Store() const
  {
    elle::Path          path;
    elle::Region        region;
    elle::String        string;

    enter();

    // create the path.
    if (path.Create(Lune::Dictionary) == elle::StatusError)
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
  /// this method erases the current user's dictionary.
  ///
  elle::Status          Dictionary::Erase() const
  {
    elle::Path          path;

    enter();

    // create the path.
    if (path.Create(Lune::Dictionary) == elle::StatusError)
      escape("unable to create the path");

    // erase the file.
    if (elle::File::Erase(path) == elle::StatusError)
      escape("unable to erase the file");

    leave();
  }

  ///
  /// this method tests the current user's dictionary.
  ///
  elle::Status          Dictionary::Exist() const
  {
    elle::Path          path;

    enter();

    // create the path.
    if (path.Create(Lune::Dictionary) == elle::StatusError)
      escape("unable to create the path");

    // test the file.
    if (elle::File::Exist(path) == elle::StatusFalse)
      false();

    true();
  }

  ///
  /// this method loads the user's local dictionary.
  ///
  elle::Status          Dictionary::Load(const elle::String&    name)
  {
    elle::Path          path;
    elle::Region        region;

    enter();

    // create the path.
    if (path.Create(Lune::User::Dictionary) == elle::StatusError)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::Piece("%USER%", name)) == elle::StatusError)
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
  /// this method stores the user's local dictionary.
  ///
  elle::Status          Dictionary::Store(const elle::String&   name) const
  {
    elle::Path          path;
    elle::Region        region;
    elle::String        string;

    enter();

    // create the path.
    if (path.Create(Lune::User::Dictionary) == elle::StatusError)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::Piece("%USER%", name)) == elle::StatusError)
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
  /// this method erases the dictionary.
  ///
  elle::Status          Dictionary::Erase(const elle::String&   name) const
  {
    elle::Path          path;

    enter();

    // create the path.
    if (path.Create(Lune::User::Dictionary) == elle::StatusError)
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
  /// this method tests the dictionary.
  ///
  elle::Status          Dictionary::Exist(const elle::String&   name) const
  {
    elle::Path          path;

    enter();

    // create the path.
    if (path.Create(Lune::User::Dictionary) == elle::StatusError)
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
