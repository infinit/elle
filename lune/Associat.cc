//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       associat
//
// file          /home/mycure/infinit/lune/Associat.cc
//
// created       julien quintard   [mon may 10 16:14:02 2010]
// updated       julien quintard   [fri may 28 18:05:34 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <lune/Associat.hh>

namespace lune
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this string defines the associat files extension.
  ///
  const elle::String		Associat::Extension = ".asct";

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method loads the user's local associat.
  ///
  elle::Status		Associat::Load()
  {
    elle::String	path =
      Lune::User::Home + elle::System::Path::Separator +
      Lune::User::Name + Associat::Extension;
    elle::Region	region;

    enter();

    // check the mode.
    if (Lune::Environment != Lune::ModeUser)
      escape("unable to manipulate associat files in this mode");

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
  /// this method stores the user's local associat.
  ///
  elle::Status		Associat::Store() const
  {
    elle::String	path =
      Lune::User::Home + elle::System::Path::Separator +
      Lune::User::Name + Associat::Extension;
    elle::Region	region;
    elle::String	string;

    enter();

    // check the mode.
    if (Lune::Environment != Lune::ModeUser)
      escape("unable to manipulate associat files in this mode");

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
  embed(Associat, _());

//
// ---------- dumpable --------------------------------------------------------
//

  ///
  /// this method dumps the associat.
  ///
  elle::Status		Associat::Dump(const elle::Natural32	margin) const
  {
    elle::String	alignment(margin, ' ');

    enter();

    std::cout << alignment << "[Associat]" << std::endl;

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
  /// this method serializes a associat.
  ///
  elle::Status		Associat::Serialize(elle::Archive&	archive) const
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
  elle::Status		Associat::Extract(elle::Archive&	archive)
  {
    enter();

    // extract the maps.
    if (archive.Extract(this->users, this->groups) == elle::StatusError)
      escape("unable to extract the maps");

    leave();
  }

}
