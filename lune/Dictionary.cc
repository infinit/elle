#include <lune/Dictionary.hh>
#include <lune/Lune.hh>

#include <elle/io/File.hh>
#include <elle/io/Piece.hh>

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
// ---------- methods ---------------------------------------------------------
//

  elle::io::Path
  Dictionary::_path(elle::String const& user)
  {
    return (elle::io::Path(Lune::User::Dictionary,
                           elle::io::Piece("%USER%", user)));
  }

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

  void
  Dictionary::load(elle::String const& user)
  {
    this->load(Dictionary::_path(user));
  }

  void
  Dictionary::store(elle::String const& user) const
  {
    this->store(Dictionary::_path(user));
  }

  void
  Dictionary::erase(elle::String const& user)
  {
    elle::concept::Fileable<>::erase(Dictionary::_path(user));
  }

  elle::Boolean
  Dictionary::exists(elle::String const& user)
  {
    return (elle::concept::Fileable<>::exists(Dictionary::_path(user)));
  }

}
