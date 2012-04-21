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

    std::cout << alignment << "[Dictionary]" << std::endl;

    // dump the users map.
    if (this->users.Dump(margin + 2) == elle::Status::Error)
      escape("unable to dump the users map");

    // dump the groups map.
    if (this->groups.Dump(margin + 2) == elle::Status::Error)
      escape("unable to dump the groups map");

    return elle::Status::Ok;
  }

//
// ---------- archivable ------------------------------------------------------
//

  ///
  /// this method serializes a dictionary.
  ///
  //elle::Status          Dictionary::Serialize(elle::Archive&    archive) const
  //{
  //  // serialize the maps.
  //  if (archive.Serialize(this->users, this->groups) == elle::Status::Error)
  //    escape("unable to serialize the maps");

  //  return elle::Status::Ok;
  //}

  /////
  ///// this method extract a public key from the given archive.
  /////
  //elle::Status          Dictionary::Extract(elle::Archive&      archive)
  //{
  //  // extract the maps.
  //  if (archive.Extract(this->users, this->groups) == elle::Status::Error)
  //    escape("unable to extract the maps");

  //  return elle::Status::Ok;
  //}

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

    // create the path.
    if (path.Create(Lune::Dictionary) == elle::Status::Error)
      escape("unable to create the path");

    // read the file's content.
    if (elle::io::File::Read(path, region) == elle::Status::Error)
      escape("unable to read the file's content");

    // decode and extract the object.
    if (elle::Hexadecimal::Decode(
          elle::String(reinterpret_cast<char*>(region.contents),
                       region.size),
          *this) == elle::Status::Error)
      escape("unable to decode the object");

    return elle::Status::Ok;
  }

  ///
  /// this method stores the current user's local dictionary.
  ///
  elle::Status          Dictionary::Store() const
  {
    elle::Path          path;
    elle::Region        region;
    elle::String        string;

    // create the path.
    if (path.Create(Lune::Dictionary) == elle::Status::Error)
      escape("unable to create the path");

    // encode in hexadecimal.
    if (elle::Hexadecimal::Encode(*this, string) == elle::Status::Error)
      escape("unable to encode the object in hexadecimal");

    // wrap the string.
    if (region.Wrap(reinterpret_cast<const elle::Byte*>(string.c_str()),
                    string.length()) == elle::Status::Error)
      escape("unable to wrap the string in a region");

    // write the file's content.
    if (elle::io::File::Write(path, region) == elle::Status::Error)
      escape("unable to write the file's content");

    return elle::Status::Ok;
  }

  ///
  /// this method erases the current user's dictionary.
  ///
  elle::Status          Dictionary::Erase() const
  {
    elle::Path          path;

    // create the path.
    if (path.Create(Lune::Dictionary) == elle::Status::Error)
      escape("unable to create the path");

    // erase the file.
    if (elle::io::File::Erase(path) == elle::Status::Error)
      escape("unable to erase the file");

    return elle::Status::Ok;
  }

  ///
  /// this method tests the current user's dictionary.
  ///
  elle::Status          Dictionary::Exist() const
  {
    elle::Path          path;

    // create the path.
    if (path.Create(Lune::Dictionary) == elle::Status::Error)
      escape("unable to create the path");

    // test the file.
    if (elle::io::File::Exist(path) == elle::Status::False)
      return elle::Status::False;

    return elle::Status::True;
  }

  ///
  /// this method loads the user's local dictionary.
  ///
  elle::Status          Dictionary::Load(const elle::String&    name)
  {
    elle::Path          path;
    elle::Region        region;

    // create the path.
    if (path.Create(Lune::User::Dictionary) == elle::Status::Error)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::io::Piece("%USER%", name)) == elle::Status::Error)
      escape("unable to complete the path");

    // read the file's content.
    if (elle::io::File::Read(path, region) == elle::Status::Error)
      escape("unable to read the file's content");

    // decode and extract the object.
    if (elle::Hexadecimal::Decode(
          elle::String(reinterpret_cast<char*>(region.contents),
                       region.size),
          *this) == elle::Status::Error)
      escape("unable to decode the object");

    return elle::Status::Ok;
  }

  ///
  /// this method stores the user's local dictionary.
  ///
  elle::Status          Dictionary::Store(const elle::String&   name) const
  {
    elle::Path          path;
    elle::Region        region;
    elle::String        string;

    // create the path.
    if (path.Create(Lune::User::Dictionary) == elle::Status::Error)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::io::Piece("%USER%", name)) == elle::Status::Error)
      escape("unable to complete the path");

    // encode in hexadecimal.
    if (elle::Hexadecimal::Encode(*this, string) == elle::Status::Error)
      escape("unable to encode the object in hexadecimal");

    // wrap the string.
    if (region.Wrap(reinterpret_cast<const elle::Byte*>(string.c_str()),
                    string.length()) == elle::Status::Error)
      escape("unable to wrap the string in a region");

    // write the file's content.
    if (elle::io::File::Write(path, region) == elle::Status::Error)
      escape("unable to write the file's content");

    return elle::Status::Ok;
  }

  ///
  /// this method erases the dictionary.
  ///
  elle::Status          Dictionary::Erase(const elle::String&   name) const
  {
    elle::Path          path;

    // create the path.
    if (path.Create(Lune::User::Dictionary) == elle::Status::Error)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::io::Piece("%USER%", name)) == elle::Status::Error)
      escape("unable to complete the path");

    // erase the file.
    if (elle::io::File::Erase(path) == elle::Status::Error)
      escape("unable to erase the file");

    return elle::Status::Ok;
  }

  ///
  /// this method tests the dictionary.
  ///
  elle::Status          Dictionary::Exist(const elle::String&   name) const
  {
    elle::Path          path;

    // create the path.
    if (path.Create(Lune::User::Dictionary) == elle::Status::Error)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::io::Piece("%USER%", name)) == elle::Status::Error)
      escape("unable to complete the path");

    // test the file.
    if (elle::io::File::Exist(path) == elle::Status::False)
      return elle::Status::False;

    return elle::Status::True;
  }

}
