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
// updated       julien quintard   [fri may 28 18:18:03 2010]
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
  /// this variable contains the path to the Infinit system root directory.
  ///
  elle::String			Lune::System::Root;

  ///
  /// this variable contains the path to the identities directory.
  ///
  elle::String			Lune::System::Identities;

  ///
  /// this variable contains the path to the universes directory.
  ///
  elle::String			Lune::System::Universes;

  ///
  /// this variable contains the path of the passports directory.
  ///
  elle::String			Lune::System::Passports;

  ///
  /// this variable contains the path to the authority file.
  ///
  elle::String			Lune::System::Authority;



  ///
  /// this variable contains the path to the Infinit user home directory.
  ///
  elle::String			Lune::User::Home;

  ///
  /// this variable contains the path to the universes directory.
  ///
  elle::String			Lune::User::Universes;

  ///
  /// this variable contains the path of the passports directory.
  ///
  elle::String			Lune::User::Passports;

  ///
  /// this variable contains the path of the reserve directory.
  ///
  elle::String			Lune::User::Reserve;

  ///
  /// this variable contains the path of the hole directory.
  ///
  elle::String			Lune::User::Hole;

//
// ---------- static methods --------------------------------------------------
//

  ///
  /// this method initializes the system definitions.
  ///
  elle::Status		Lune::Initialize()
  {
    enter();

    Lune::Environment = Lune::ModeSystem;

    //
    // set the system definitions.
    //

    Lune::System::Root =
      elle::System::Path::Root + elle::System::Path::Separator + "infinit";

    Lune::System::Identities =
      Lune::System::Root + elle::System::Path::Separator + "identities";

    Lune::System::Universes =
      Lune::System::Root + elle::System::Path::Separator + "universes";

    Lune::System::Passports =
      Lune::System::Root + elle::System::Path::Separator + "passports";

    Lune::System::Authority =
      Lune::System::Root + elle::System::Path::Separator + "infinit";

    //
    // set the user definitions.
    //

    Lune::User::Home =
      elle::System::Path::Home + elle::System::Path::Separator + ".infinit";

    Lune::User::Universes =
      Lune::User::Home + elle::System::Path::Separator + "universes";

    Lune::User::Passports =
      Lune::User::Home + elle::System::Path::Separator + "passports";

    Lune::User::Reserve =
      Lune::User::Home + elle::System::Path::Separator + "reserve";

    Lune::User::Hole =
      Lune::User::Home + elle::System::Path::Separator + "hole";

    leave();
  }

  ///
  /// this method selects the user environment.
  ///
  elle::Status		Lune::Select(const elle::String&	name)
  {
    enter();

    // set the mode.
    Lune::Environment = Lune::ModeUser;

    // set the user name.
    Lune::User::Name = name;

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
