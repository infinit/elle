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
// updated       julien quintard   [sun may  8 11:03:40 2011]
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
  /// this variable contains the path to the Infinit authority
  /// file.
  ///
  elle::Pattern			Lune::Authority;

  ///
  /// this variable contains the path to the users directory.
  ///
  elle::Pattern			Lune::Users;

  ///
  /// this variable contains path to the networks directory.
  ///
  elle::Pattern			Lune::Networks;

  ///
  /// this variable contains the pattern-based path to a specific user
  /// directory.
  ///
  elle::Pattern			Lune::User::Root;

  ///
  /// this variable contains the pattern-based path to the user identity file.
  ///
  elle::Pattern			Lune::User::Identity;

  ///
  /// this variable contains the pattern-based path to the user dictionary
  /// file.
  ///
  elle::Pattern			Lune::User::Dictionary;

  ///
  /// this variable contains the pattern-based path to the user phrase
  /// file.
  ///
  elle::Pattern			Lune::User::Phrase;

  ///
  /// this variable contains the pattern-based path to a specific network
  /// directory.
  ///
  elle::Pattern			Lune::Network::Root;

  ///
  /// this variable holds the pattern of the path leading to a given
  /// network descriptor.
  ///
  elle::Pattern			Lune::Network::Descriptor;

  ///
  /// this variable holds the pattern of the path leading to a given
  /// network's reserve.
  ///
  elle::Pattern			Lune::Network::Reserve::Root;

  ///
  /// this variable holds the pattern of the path leading to a specific
  /// block within the network's reserve.
  ///
  elle::Pattern			Lune::Network::Reserve::Block;

  ///
  /// this variable holds the pattern of the path leading to a given
  /// network's shelter.
  ///
  elle::Pattern			Lune::Network::Shelter::Root;

  ///
  /// this variable holds the pattern of the path leading to a specific
  /// block within the network's reserve.
  ///
  elle::Pattern			Lune::Network::Shelter::Block;

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

    // create the users path pattern.
    if (Lune::Users.Create(home +
			   elle::System::Path::Separator +
			   "users") == elle::StatusError)
      escape("unable to create the pattern");

    // create the networks path pattern.
    if (Lune::Networks.Create(home +
			      elle::System::Path::Separator +
			      "networks") == elle::StatusError)
      escape("unable to create the pattern");

    // create the user path pattern.
    if (Lune::User::Root.Create(home +
				elle::System::Path::Separator +
				"users" +
				elle::System::Path::Separator +
				"%USER%") == elle::StatusError)
      escape("unable to create the pattern");

    // create the identity path pattern.
    if (Lune::User::Identity.Create(home +
				    elle::System::Path::Separator +
				    "users" +
				    elle::System::Path::Separator +
				    "%USER%" +
				    elle::System::Path::Separator +
				    "%USER%" +
				    Identity::Extension) == elle::StatusError)
      escape("unable to create the pattern");

    // create the dictionary path pattern.
    if (Lune::User::Dictionary.Create(home +
				      elle::System::Path::Separator +
				      "users" +
				      elle::System::Path::Separator +
				      "%USER%" +
				      elle::System::Path::Separator +
				      "%USER%" +
				      Dictionary::Extension) ==
	elle::StatusError)
      escape("unable to create the pattern");

    // create the phrase path pattern.
    if (Lune::User::Phrase.Create(home +
				  elle::System::Path::Separator +
				  "users" +
				  elle::System::Path::Separator +
				  "%USER%" +
				  elle::System::Path::Separator +
				  "%USER%" +
				  Phrase::Extension) == elle::StatusError)
      escape("unable to create the pattern");

    // create the network path pattern.
    if (Lune::Network::Root.Create(home +
				   elle::System::Path::Separator +
				   "networks" +
				   elle::System::Path::Separator +
				   "%NETWORK%") == elle::StatusError)
      escape("unable to create the pattern");

    // create the descriptor path pattern.
    if (Lune::Network::Descriptor.Create(home +
					 elle::System::Path::Separator +
					 "networks" +
					 elle::System::Path::Separator +
					 "%NETWORK%" +
					 elle::System::Path::Separator +
					 "%NETWORK%" +
					 Descriptor::Extension) ==
	elle::StatusError)
      escape("unable to create the pattern");

    // create the reserve path pattern.
    if (Lune::Network::Reserve::Root.Create(home +
					    elle::System::Path::Separator +
					    "networks" +
					    elle::System::Path::Separator +
					    "%NETWORK%" +
					    elle::System::Path::Separator +
					    "reserve") == elle::StatusError)
      escape("unable to create the pattern");

    // create the block pattern within the reserve.
    if (Lune::Network::Reserve::Block.Create(home +
					     elle::System::Path::Separator +
					     "networks" +
					     elle::System::Path::Separator +
					     "%NETWORK%" +
					     elle::System::Path::Separator +
					     "reserve" +
					     elle::System::Path::Separator +
					     "%ADDRESS%" +
					     nucleus::Block::Extension) ==
	elle::StatusError)
      escape("unable to create the pattern");

    // create the shelter path pattern.
    if (Lune::Network::Shelter::Root.Create(home +
					    elle::System::Path::Separator +
					    "networks" +
					    elle::System::Path::Separator +
					    "%NETWORK%" +
					    elle::System::Path::Separator +
					    "shelter") == elle::StatusError)
      escape("unable to create the pattern");

    // create the block pattern within the shelter.
    if (Lune::Network::Shelter::Block.Create(home +
					     elle::System::Path::Separator +
					     "networks" +
					     elle::System::Path::Separator +
					     "%NETWORK%" +
					     elle::System::Path::Separator +
					     "shelter" +
					     elle::System::Path::Separator +
					     "%ADDRESS%" +
					     nucleus::Block::Extension) ==
	elle::StatusError)
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
