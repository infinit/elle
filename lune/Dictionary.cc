#include <lune/Dictionary.hh>
#include <lune/Lune.hh>

#include <elle/os/path.hh>
#include <elle/io/File.hh>
#include <elle/io/Piece.hh>

namespace lune
{

//
// ---------- methods ---------------------------------------------------------
//

  elle::io::Path
  Dictionary::_path(elle::String const& user_id)
  {
    return (elle::io::Path(Lune::Dictionary,
                           elle::io::Piece("%USER%", user_id)));
  }

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
      throw elle::Exception("unable to dump the users map");

    // dump the groups map.
    if (this->groups.Dump(margin + 2) == elle::Status::Error)
      throw elle::Exception("unable to dump the groups map");

    return elle::Status::Ok;
  }

//
// ---------- fileable --------------------------------------------------------
//

  void
  Dictionary::load(elle::String const& user_id)
  {
    this->load(Dictionary::_path(user_id));
  }

  void
  Dictionary::store(elle::String const& user_id) const
  {
    this->store(Dictionary::_path(user_id));
  }

  void
  Dictionary::erase(elle::String const& user_id)
  {
    elle::concept::Fileable<>::erase(Dictionary::_path(user_id));
  }

  elle::Boolean
  Dictionary::exists(elle::String const& user_id)
  {
    return (elle::os::path::exists(Dictionary::_path(user_id).string()));
  }

}
