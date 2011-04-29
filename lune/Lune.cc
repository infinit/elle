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
// updated       julien quintard   [tue apr 26 15:12:16 2011]
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
  /// this variable contains the pattern-based path to the Infinit authority
  /// file.
  ///
  elle::Pattern			Lune::Authority;

  ///
  /// this variable contains the pattern-based path to a specific user
  /// directory.
  ///
  elle::Pattern			Lune::User;

  ///
  /// this variable contains the pattern-based path to the user identity file.
  ///
  elle::Pattern			Lune::Identity;

  ///
  /// this variable contains the pattern-based path to the user dictionary
  /// file.
  ///
  elle::Pattern			Lune::Dictionary;

  ///
  /// this variable contains the pattern-based path to the user phrase
  /// file.
  ///
  elle::Pattern			Lune::Phrase;

  ///
  /// this variable contains the pattern-based path to a specific network
  /// directory.
  ///
  elle::Pattern			Lune::Network;

  ///
  /// this variable holds the pattern of the path leading to a given
  /// network descriptor.
  ///
  elle::Pattern			Lune::Descriptor;

  ///
  /// this variable holds the pattern of the path leading to a given
  /// network's reserve.
  ///
  elle::Pattern			Lune::Reserve;

  ///
  /// this variable holds the pattern of the path leading to a given
  /// network's shelter.
  ///
  elle::Pattern			Lune::Shelter;

//
// ---------- static methods --------------------------------------------------
//

  ///
  /// this method initializes the system definitions.
  ///
  elle::Status		Lune::Initialize()
  {
    elle::String	home =
      elle::System::Path::Home +
      elle::System::Path::Separator +
      ".infinit";

    enter();

    // create the authority path pattern.
    if (Lune::Authority.Create(home +
			       elle::System::Path::Separator +
			       "infinit" +
			       Authority::Extension) == elle::StatusError)
      escape("unable to create the pattern");

    // create the user path pattern.
    if (Lune::User.Create(home +
			  elle::System::Path::Separator +
			  "users" +
			  elle::System::Path::Separator +
			  "%USER%") == elle::StatusError)
      escape("unable to create the pattern");

    // create the identity path pattern.
    if (Lune::Identity.Create(home +
			      elle::System::Path::Separator +
			      "users" +
			      elle::System::Path::Separator +
			      "%USER%" +
			      elle::System::Path::Separator +
			      "%USER%" +
			      Identity::Extension) == elle::StatusError)
      escape("unable to create the pattern");

    // create the dictionary path pattern.
    if (Lune::Dictionary.Create(home +
				elle::System::Path::Separator +
				"users" +
				elle::System::Path::Separator +
				"%USER%" +
				elle::System::Path::Separator +
				"%USER%" +
				Dictionary::Extension) == elle::StatusError)
      escape("unable to create the pattern");

    // create the phrase path pattern.
    if (Lune::Phrase.Create(home +
			    elle::System::Path::Separator +
			    "users" +
			    elle::System::Path::Separator +
			    "%USER%" +
			    elle::System::Path::Separator +
			    "%USER%" +
			    Phrase::Extension) == elle::StatusError)
      escape("unable to create the pattern");

    // create the network path pattern.
    if (Lune::Network.Create(home +
			     elle::System::Path::Separator +
			     "networks" +
			     elle::System::Path::Separator +
			     "%NETWORK%") == elle::StatusError)
      escape("unable to create the pattern");

    // create the descriptor path pattern.
    if (Lune::Descriptor.Create(home +
				elle::System::Path::Separator +
				"networks" +
				elle::System::Path::Separator +
				"%NETWORK%" +
				elle::System::Path::Separator +
				"%NETWORK%" +
				Descriptor::Extension) == elle::StatusError)
      escape("unable to create the pattern");

    // create the reserve path pattern.
    if (Lune::Reserve.Create(home +
			     elle::System::Path::Separator +
			     "networks" +
			     elle::System::Path::Separator +
			     "%NETWORK%" +
			     elle::System::Path::Separator +
			     "reserve") == elle::StatusError)
      escape("unable to create the pattern");

    // create the shelter path pattern.
    if (Lune::Shelter.Create(home +
			     elle::System::Path::Separator +
			     "networks" +
			     elle::System::Path::Separator +
			     "%NETWORK%" +
			     elle::System::Path::Separator +
			     "shelter") == elle::StatusError)
      escape("unable to create the pattern");

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
