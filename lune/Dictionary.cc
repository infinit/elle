#include <lune/Dictionary.hh>
#include <lune/Lune.hh>

#include <elle/io/File.hh>
#include <elle/io/Piece.hh>
#include <elle/cryptography/PublicKey.hh>

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
// ---------- fileable --------------------------------------------------------
//

  ///
  /// this method loads the user's local dictionary.
  ///
  elle::Status          Dictionary::Load(const elle::String&    name)
  {
    elle::io::Path          path;
    elle::standalone::Region        region;

    // create the path.
    if (path.Create(Lune::User::Dictionary) == elle::Status::Error)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::io::Piece("%USER%", name)) == elle::Status::Error)
      escape("unable to complete the path");

    if (this->Load(path) == elle::Status::Error)
      escape("unable to decode the object");

    return elle::Status::Ok;
  }

  ///
  /// this method stores the user's local dictionary.
  ///
  elle::Status          Dictionary::Store(const elle::String&   name) const
  {
    elle::io::Path          path;
    elle::standalone::Region        region;
    elle::String        string;

    // create the path.
    if (path.Create(Lune::User::Dictionary) == elle::Status::Error)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::io::Piece("%USER%", name)) == elle::Status::Error)
      escape("unable to complete the path");

    if (this->Store(path) == elle::Status::Error)
      escape("unable to store the object");

    return elle::Status::Ok;
  }

  ///
  /// this method erases the dictionary.
  ///
  elle::Status          Dictionary::Erase(const elle::String&   name) const
  {
    elle::io::Path          path;

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
    elle::io::Path          path;

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
