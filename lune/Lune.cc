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
// updated       julien quintard   [mon may  3 11:57:36 2010]
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
  /// this variable contains the path to the keys directory.
  ///
  elle::String			Lune::Keys;

  ///
  /// this variable contains the path to the universes directory.
  ///
  elle::String			Lune::Universes;

  ///
  /// this variable contains the path to the reserve directory.
  ///
  elle::String			Lune::Reserve;

  ///
  /// this variable contains the path to the hole directory.
  ///
  elle::String			Lune::Hole;

  ///
  /// this variable contains the path to the phrase file.
  ///
  elle::String			Lune::Phrase;

  ///
  /// this variable contains the path to the identity file.
  ///
  elle::String			Lune::Identity;

  ///
  /// this variable contains the path to the accesses file.
  ///
  elle::String			Lune::Accesses;

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

    Lune::Keys =
      Lune::Home + elle::System::Path::Separator + "keys";

    Lune::Universes =
      Lune::Home + elle::System::Path::Separator + "universes";

    Lune::Reserve =
      Lune::Home + elle::System::Path::Separator + "reserve";

    Lune::Hole =
      Lune::Home + elle::System::Path::Separator + "hole";

    Lune::Phrase =
      Lune::Home + elle::System::Path::Separator + "phrase.txt";

    Lune::Identity =
      Lune::Home + elle::System::Path::Separator + "identity.txt";

    Lune::Accesses =
      Lune::Home + elle::System::Path::Separator + "accesses.txt";

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
