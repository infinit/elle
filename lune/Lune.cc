//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// author        julien quintard   [fri apr 30 16:29:27 2010]
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
  /// this variable contains the path to the Infinit directory.
  ///
  elle::Pattern                 Lune::Home;

  ///
  /// this variable contains the path to the Infinit authority
  /// file.
  ///
  elle::Pattern                 Lune::Authority;

  ///
  /// this variable contains the path to the Infinit passport
  /// file.
  ///
  elle::Pattern                 Lune::Passport;

  ///
  /// this variable contains the pattern-based path to the configuration
  /// file.
  ///
  elle::Pattern                 Lune::Configuration;

  ///
  /// this variable contains the pattern-based path to the log
  /// file.
  ///
  elle::Pattern                 Lune::Log;

  ///
  /// this variable contains the pattern-based path to the user identity file.
  ///
  elle::Pattern                 Lune::Identity;

  ///
  /// this variable contains the pattern-based path to the user dictionary
  /// file.
  ///
  elle::Pattern                 Lune::Dictionary;

  ///
  /// this variable contains path to the users directory.
  ///
  elle::Pattern                 Lune::Users;

  ///
  /// this variable contains the pattern-based path to a specific user
  /// directory
  ///
  elle::Pattern                 Lune::User::Root;

  ///
  /// this variable holds the pattern of the path leading to a given
  /// user identity.
  ///
  elle::Pattern                 Lune::User::Identity;

  ///
  /// this variable holds the pattern of the path leading to a given
  /// user dictionary.
  ///
  elle::Pattern                 Lune::User::Dictionary;

  ///
  /// this variable contains path to the networks directory.
  ///
  elle::Pattern                 Lune::Networks;

  ///
  /// this variable contains the pattern-based path to a specific network
  /// directory.
  ///
  elle::Pattern                 Lune::Network::Root;

  ///
  /// this variable holds the pattern of the path leading to a given
  /// network descriptor.
  ///
  elle::Pattern                 Lune::Network::Descriptor;

  ///
  /// this variable contains the pattern-based path to the network-specific
  /// phrase file.
  ///
  elle::Pattern                 Lune::Network::Phrase;

  ///
  /// this variable holds the pattern of the path leading to a given
  /// network's reserve.
  ///
  elle::Pattern                 Lune::Network::Reserve::Root;

  ///
  /// this variable holds the pattern of the path leading to a specific
  /// immutable block within the network's reserve.
  ///
  elle::Pattern                 Lune::Network::Reserve::ImmutableBlock;

  ///
  /// this variable holds the pattern of the path leading to a specific
  /// mutable block within the network's reserve.
  ///
  elle::Pattern                 Lune::Network::Reserve::MutableBlock;

  ///
  /// this variable holds the pattern of the path leading to a specific
  /// mutable block's history within the network's reserve.
  ///
  elle::Pattern                 Lune::Network::Reserve::History;

  ///
  /// this variable holds the pattern of the path leading to a given
  /// network's shelter.
  ///
  elle::Pattern                 Lune::Network::Shelter::Root;

  ///
  /// this variable holds the pattern of the path leading to a specific
  /// immutable block within the network's shelter.
  ///
  elle::Pattern                 Lune::Network::Shelter::ImmutableBlock;

  ///
  /// this variable holds the pattern of the path leading to a specific
  /// mutable block within the network's shelter.
  ///
  elle::Pattern                 Lune::Network::Shelter::MutableBlock;

  ///
  /// this variable holds the pattern of the path leading to a specific
  /// mutable block's history within the network's shelter.
  ///
  elle::Pattern                 Lune::Network::Shelter::History;

//
// ---------- static methods --------------------------------------------------
//

  ///
  /// this method initializes the system definitions.
  ///
  elle::Status          Lune::Initialize()
  {
    elle::String        home =
      elle::System::Path::Home +
      elle::System::Path::Separator +
      ".config" +
      elle::System::Path::Separator +
      "infinit";

    ;

    //
    // create the paths.
    //
    {
      // create the home path pattern.
      if (Lune::Home.Create(
            home) == elle::StatusError)
        escape("unable to create the pattern");

      // create the authority path pattern.
      if (Lune::Authority.Create(
            home +
            elle::System::Path::Separator +
            "infinit" +
            Authority::Extension) == elle::StatusError)
        escape("unable to create the pattern");

      // create the passport path pattern.
      if (Lune::Passport.Create(
            home +
            elle::System::Path::Separator +
            "infinit" +
            Passport::Extension) == elle::StatusError)
        escape("unable to create the pattern");

      // create the configuration path pattern.
      if (Lune::Configuration.Create(
            home +
            elle::System::Path::Separator +
            "infinit" +
            Configuration::Extension) == elle::StatusError)
        escape("unable to create the pattern");

      // create the log path pattern.
      if (Lune::Log.Create(
            home +
            elle::System::Path::Separator +
            "infinit" +
            Log::Extension) == elle::StatusError)
        escape("unable to create the pattern");

      // create the identity path pattern.
      if (Lune::Identity.Create(
            home +
            elle::System::Path::Separator +
            "infinit" +
            Identity::Extension) == elle::StatusError)
        escape("unable to create the pattern");

      // create the dictionary path pattern.
      if (Lune::Dictionary.Create(
            home +
            elle::System::Path::Separator +
            "infinit" +
            Dictionary::Extension) == elle::StatusError)
        escape("unable to create the pattern");

      // create the users path pattern.
      if (Lune::Users.Create(
            home +
            elle::System::Path::Separator +
            "users") == elle::StatusError)
        escape("unable to create the pattern");

      // create the user path pattern.
      if (Lune::User::Root.Create(
            home +
            elle::System::Path::Separator +
            "users" +
            elle::System::Path::Separator +
            "%USER%") == elle::StatusError)
        escape("unable to create the pattern");

      // create the identity path pattern.
      if (Lune::User::Identity.Create(
            home +
            elle::System::Path::Separator +
            "users" +
            elle::System::Path::Separator +
            "%USER%" +
            elle::System::Path::Separator +
            "%USER%" +
            Identity::Extension) == elle::StatusError)
        escape("unable to create the pattern");

      // create the dictionary path pattern.
      if (Lune::User::Dictionary.Create(
            home +
            elle::System::Path::Separator +
            "users" +
            elle::System::Path::Separator +
            "%USER%" +
            elle::System::Path::Separator +
            "%USER%" +
            Dictionary::Extension) == elle::StatusError)
        escape("unable to create the pattern");

      // create the networks path pattern.
      if (Lune::Networks.Create(
            home +
            elle::System::Path::Separator +
            "networks") == elle::StatusError)
        escape("unable to create the pattern");

      // create the network path pattern.
      if (Lune::Network::Root.Create(
            home +
            elle::System::Path::Separator +
            "networks" +
            elle::System::Path::Separator +
            "%NETWORK%") == elle::StatusError)
        escape("unable to create the pattern");

      // create the descriptor path pattern.
      if (Lune::Network::Descriptor.Create(
            home +
            elle::System::Path::Separator +
            "networks" +
            elle::System::Path::Separator +
            "%NETWORK%" +
            elle::System::Path::Separator +
            "%NETWORK%" +
            Descriptor::Extension) == elle::StatusError)
        escape("unable to create the pattern");

      // create the phrase path pattern.
      if (Lune::Network::Phrase.Create(
            home +
            elle::System::Path::Separator +
            "networks" +
            elle::System::Path::Separator +
            "%NETWORK%" +
            elle::System::Path::Separator +
            "%NETWORK%" +
            Phrase::Extension) == elle::StatusError)
        escape("unable to create the pattern");

      // create the reserve path pattern.
      if (Lune::Network::Reserve::Root.Create(
            home +
            elle::System::Path::Separator +
            "networks" +
            elle::System::Path::Separator +
            "%NETWORK%" +
            elle::System::Path::Separator +
            "reserve") == elle::StatusError)
        escape("unable to create the pattern");

      // create the immutable block pattern within the reserve.
      if (Lune::Network::Reserve::ImmutableBlock.Create(
            home +
            elle::System::Path::Separator +
            "networks" +
            elle::System::Path::Separator +
            "%NETWORK%" +
            elle::System::Path::Separator +
            "reserve" +
            elle::System::Path::Separator +
            "%ADDRESS%" +
            nucleus::Block::Extension) == elle::StatusError)
        escape("unable to create the pattern");

      // create the mutable block pattern within the reserve.
      if (Lune::Network::Reserve::MutableBlock.Create(
            home +
            elle::System::Path::Separator +
            "networks" +
            elle::System::Path::Separator +
            "%NETWORK%" +
            elle::System::Path::Separator +
            "reserve" +
            elle::System::Path::Separator +
            "%ADDRESS%" +
            "#" +
            "%VERSION%" +
            nucleus::Block::Extension) == elle::StatusError)
        escape("unable to create the pattern");

      // create the mutable block's history pattern within the reserve.
      if (Lune::Network::Reserve::History.Create(
            home +
            elle::System::Path::Separator +
            "networks" +
            elle::System::Path::Separator +
            "%NETWORK%" +
            elle::System::Path::Separator +
            "reserve" +
            elle::System::Path::Separator +
            "%ADDRESS%" +
            nucleus::History::Extension) == elle::StatusError)
        escape("unable to create the pattern");

      // create the shelter path pattern.
      if (Lune::Network::Shelter::Root.Create(
            home +
            elle::System::Path::Separator +
            "networks" +
            elle::System::Path::Separator +
            "%NETWORK%" +
            elle::System::Path::Separator +
            "shelter") == elle::StatusError)
        escape("unable to create the pattern");

      // create the immutable block pattern within the shelter.
      if (Lune::Network::Shelter::ImmutableBlock.Create(
            home +
            elle::System::Path::Separator +
            "networks" +
            elle::System::Path::Separator +
            "%NETWORK%" +
            elle::System::Path::Separator +
            "shelter" +
            elle::System::Path::Separator +
            "%ADDRESS%" +
            nucleus::Block::Extension) == elle::StatusError)
        escape("unable to create the pattern");

      // create the mutable block pattern within the shelter.
      if (Lune::Network::Shelter::MutableBlock.Create(
            home +
            elle::System::Path::Separator +
            "networks" +
            elle::System::Path::Separator +
            "%NETWORK%" +
            elle::System::Path::Separator +
            "shelter" +
            elle::System::Path::Separator +
            "%ADDRESS%" +
            "#" +
            "%VERSION%" +
            nucleus::Block::Extension) == elle::StatusError)
        escape("unable to create the pattern");

      // create the mutable block's history pattern within the shelter.
      if (Lune::Network::Shelter::History.Create(
            home +
            elle::System::Path::Separator +
            "networks" +
            elle::System::Path::Separator +
            "%NETWORK%" +
            elle::System::Path::Separator +
            "shelter" +
            elle::System::Path::Separator +
            "%ADDRESS%" +
            nucleus::History::Extension) == elle::StatusError)
        escape("unable to create the pattern");
    }

    //
    // setup the log mechanism
    //
    {
      // setup the log.
      if (elle::Log::Setup(Lune::Log.string) == elle::StatusError)
        escape("unable to set up the log system");
    }

    return elle::StatusOk;
  }

  ///
  /// this method cleans the static definitions.
  ///
  elle::Status          Lune::Clean()
  {
    ;

    // nothing to do.

    return elle::StatusOk;
  }

}
