//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// file          /home/mycure/infinit/lune/Lune.cc
//
// created       julien quintard   [fri apr 30 16:29:27 2010]
// updated       julien quintard   [wed may  5 20:09:56 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <lune/Lune.hh>

namespace lune
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this variable contains the path to the user's Infinit home directory.
  ///
  elle::String			Lune::Home;

  ///
  /// this variable contains the path to the users directory.
  ///
  elle::String			Lune::Users;

  ///
  /// this variable contains the name of the passports directory.
  ///
  elle::String			Lune::Passports;

  ///
  /// this variable contains the path to the universes directory.
  ///
  elle::String			Lune::Universes;

  ///
  /// this variable contains the name of the reserve directory.
  ///
  elle::String			Lune::Reserve;

  ///
  /// this variable contains the name of the hole directory.
  ///
  elle::String			Lune::Hole;

  ///
  /// this variable contains the path to the authority file.
  ///
  elle::String			Lune::Authority;

  ///
  /// this variable contains the path to the phrase file.
  ///
  elle::String			Lune::Phrase;

  ///
  /// this variable contains the path to the access file.
  ///
  elle::String			Lune::Access;

//
// ---------- static methods --------------------------------------------------
//

  ///
  /// this method initializes the static definitions.
  ///
  elle::Status		Lune::Initialize()
  {
    enter();

    //
    // set the static definitions.
    //

    Lune::Home =
      elle::System::Path::Home + elle::System::Path::Separator + ".infinit";

    Lune::Users =
      Lune::Home + elle::System::Path::Separator + "users";

    Lune::Passports = "passports";

    Lune::Universes =
      Lune::Home + elle::System::Path::Separator + "universes";

    Lune::Reserve = "reserve";

    Lune::Hole = "hole";

    Lune::Authority =
      Lune::Home + elle::System::Path::Separator + "infinit";

    Lune::Phrase =
      Lune::Home + elle::System::Path::Separator + "phrase";

    Lune::Access =
      Lune::Home + elle::System::Path::Separator + "access";

    leave();
  }

  ///
  /// this method cleans the static definitions.
  ///
  elle::Status		Lune::Clean()
  {
    enter();

    // nothing to do.

    leave();
  }

}
