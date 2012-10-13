#include <lune/Lune.hh>
#include <lune/Authority.hh>
#include <lune/Passport.hh>
#include <lune/Configuration.hh>
#include <lune/Identity.hh>
#include <lune/Dictionary.hh>
#include <lune/Descriptor.hh>
#include <lune/Set.hh>
#include <lune/Phrase.hh>

#include <nucleus/proton/History.hh>
#include <nucleus/proton/Block.hh>

#include <elle/io/Path.hh>
#include <elle/io/Piece.hh>
#include <elle/system/system.hh>

#include <common/common.hh>

#include <Infinit.hh>

namespace lune
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this variable contains the path to the Infinit directory.
  ///
  elle::io::Pattern                 Lune::Home;

  ///
  /// this variable contains the path to the Infinit authority
  /// file.
  ///
  elle::io::Pattern                 Lune::Authority;

  ///
  /// this variable contains the path to the Infinit passport
  /// file.
  ///
  elle::io::Pattern                 Lune::Passport;

  ///
  /// this variable contains the pattern-based path to the configuration
  /// file.
  ///
  elle::io::Pattern                 Lune::Configuration;

  elle::io::Pattern Lune::Phrase;

  ///
  /// this variable contains path to the users directory.
  ///
  elle::io::Pattern                 Lune::Users;

  ///
  /// this variable contains the pattern-based path to a specific user
  /// directory
  ///
  elle::io::Pattern                 Lune::User::Root;

  ///
  /// this variable holds the pattern of the path leading to a given
  /// user identity.
  ///
  elle::io::Pattern                 Lune::User::Identity;

  ///
  /// this variable holds the pattern of the path leading to a given
  /// user dictionary.
  ///
  elle::io::Pattern                 Lune::User::Dictionary;

  ///
  /// this variable contains path to the networks directory.
  ///
  elle::io::Pattern                 Lune::Networks;

  ///
  /// this variable contains the pattern-based path to a specific network
  /// directory.
  ///
  elle::io::Pattern                 Lune::Network::Root;

  ///
  /// this variable holds the pattern of the path leading to a given
  /// network descriptor.
  ///
  elle::io::Pattern                 Lune::Network::Descriptor;

  ///
  /// this variable holds the pattern of the path leading to a given
  /// network set of initial nodes.
  ///
  elle::io::Pattern                 Lune::Network::Set;

  ///
  /// this variable contains the pattern-based path to the network-specific
  /// phrase file.
  ///
  elle::io::Pattern                 Lune::Network::Phrase;

  ///
  /// this variable holds the pattern of the path leading to a given
  /// network's reserve.
  ///
  elle::io::Pattern                 Lune::Network::Reserve::Root;

  ///
  /// this variable holds the pattern of the path leading to a specific
  /// immutable block within the network's reserve.
  ///
  elle::io::Pattern                 Lune::Network::Reserve::ImmutableBlock;

  ///
  /// this variable holds the pattern of the path leading to a specific
  /// mutable block within the network's reserve.
  ///
  elle::io::Pattern                 Lune::Network::Reserve::MutableBlock;

  ///
  /// this variable holds the pattern of the path leading to a specific
  /// mutable block's history within the network's reserve.
  ///
  elle::io::Pattern                 Lune::Network::Reserve::History;

  ///
  /// this variable holds the pattern of the path leading to a given
  /// network's shelter.
  ///
  elle::io::Pattern                 Lune::Network::Shelter::Root;

  ///
  /// this variable holds the pattern of the path leading to a specific
  /// immutable block within the network's shelter.
  ///
  elle::io::Pattern                 Lune::Network::Shelter::ImmutableBlock;

  ///
  /// this variable holds the pattern of the path leading to a specific
  /// mutable block within the network's shelter.
  ///
  elle::io::Pattern                 Lune::Network::Shelter::MutableBlock;

  ///
  /// this variable holds the pattern of the path leading to a specific
  /// mutable block's history within the network's shelter.
  ///
  elle::io::Pattern                 Lune::Network::Shelter::History;

//
// ---------- static methods --------------------------------------------------
//

  ///
  /// this method initializes the system definitions.
  ///
  elle::Status          Lune::Initialize()
  {
    elle::String        home = common::infinit::home();

    //
    // create the paths.
    //
    {
      // create the home path pattern.
      if (Lune::Home.Create(home) == elle::Status::Error)
        escape("unable to create the pattern");

      // create the authority path pattern.
      if (Lune::Authority.Create(
            home +
            elle::system::path::separator +
            "infinit" +
            Authority::Extension) == elle::Status::Error)
        escape("unable to create the pattern");

      // create the passport path pattern.
      if (Lune::Passport.Create(
            home +
            elle::system::path::separator +
            "infinit" +
            Passport::Extension) == elle::Status::Error)
        escape("unable to create the pattern");

      // create the configuration path pattern.
      if (Lune::Configuration.Create(
            home +
            elle::system::path::separator +
            "infinit" +
            Configuration::Extension) == elle::Status::Error)
        escape("unable to create the pattern");

      if (Lune::Phrase.Create(
            home +
            elle::system::path::separator +
            "%NAME%" +
            Phrase::Extension) == elle::Status::Error)
        escape("unable to create the pattern");

      // create the users path pattern.
      if (Lune::Users.Create(
            home +
            elle::system::path::separator +
            "users") == elle::Status::Error)
        escape("unable to create the pattern");

      // create the user path pattern.
      if (Lune::User::Root.Create(
            home +
            elle::system::path::separator +
            "users" +
            elle::system::path::separator +
            "%USER%") == elle::Status::Error)
        escape("unable to create the pattern");

      // create the identity path pattern.
      if (Lune::User::Identity.Create(
            home +
            elle::system::path::separator +
            "users" +
            elle::system::path::separator +
            "%USER%" +
            elle::system::path::separator +
            "%USER%" +
            Identity::Extension) == elle::Status::Error)
        escape("unable to create the pattern");

      // create the dictionary path pattern.
      if (Lune::User::Dictionary.Create(
            home +
            elle::system::path::separator +
            "users" +
            elle::system::path::separator +
            "%USER%" +
            elle::system::path::separator +
            "%USER%" +
            Dictionary::Extension) == elle::Status::Error)
        escape("unable to create the pattern");

      // create the networks path pattern.
      if (Lune::Networks.Create(
            home +
            elle::system::path::separator +
            "networks") == elle::Status::Error)
        escape("unable to create the pattern");

      // create the network path pattern.
      if (Lune::Network::Root.Create(
            home +
            elle::system::path::separator +
            "networks" +
            elle::system::path::separator +
            "%NETWORK%") == elle::Status::Error)
        escape("unable to create the pattern");

      // create the descriptor path pattern.
      if (Lune::Network::Descriptor.Create(
            home +
            elle::system::path::separator +
            "networks" +
            elle::system::path::separator +
            "%NETWORK%" +
            elle::system::path::separator +
            "%NETWORK%" +
            Descriptor::Extension) == elle::Status::Error)
        escape("unable to create the pattern");

      // create the set path pattern.
      if (Lune::Network::Set.Create(
            home +
            elle::system::path::separator +
            "networks" +
            elle::system::path::separator +
            "%NETWORK%" +
            elle::system::path::separator +
            "%NETWORK%" +
            Set::Extension) == elle::Status::Error)
        escape("unable to create the pattern");

      // create the phrase path pattern.
      if (Lune::Network::Phrase.Create(
            home +
            elle::system::path::separator +
            "networks" +
            elle::system::path::separator +
            "%NETWORK%" +
            elle::system::path::separator +
            "%NAME%" +
            Phrase::Extension) == elle::Status::Error)
        escape("unable to create the pattern");

      // create the reserve path pattern.
      if (Lune::Network::Reserve::Root.Create(
            home +
            elle::system::path::separator +
            "networks" +
            elle::system::path::separator +
            "%NETWORK%" +
            elle::system::path::separator +
            "reserve") == elle::Status::Error)
        escape("unable to create the pattern");

      // create the immutable block pattern within the reserve.
      if (Lune::Network::Reserve::ImmutableBlock.Create(
            home +
            elle::system::path::separator +
            "networks" +
            elle::system::path::separator +
            "%NETWORK%" +
            elle::system::path::separator +
            "reserve" +
            elle::system::path::separator +
            "%ADDRESS%" +
            nucleus::proton::Block::Extension) == elle::Status::Error)
        escape("unable to create the pattern");

      // create the mutable block pattern within the reserve.
      if (Lune::Network::Reserve::MutableBlock.Create(
            home +
            elle::system::path::separator +
            "networks" +
            elle::system::path::separator +
            "%NETWORK%" +
            elle::system::path::separator +
            "reserve" +
            elle::system::path::separator +
            "%ADDRESS%" +
            "#" +
            "%REVISION%" +
            nucleus::proton::Block::Extension) == elle::Status::Error)
        escape("unable to create the pattern");

      // create the mutable block's history pattern within the reserve.
      if (Lune::Network::Reserve::History.Create(
            home +
            elle::system::path::separator +
            "networks" +
            elle::system::path::separator +
            "%NETWORK%" +
            elle::system::path::separator +
            "reserve" +
            elle::system::path::separator +
            "%ADDRESS%" +
            nucleus::proton::History::Extension) == elle::Status::Error)
        escape("unable to create the pattern");

      // create the shelter path pattern.
      if (Lune::Network::Shelter::Root.Create(
            home +
            elle::system::path::separator +
            "networks" +
            elle::system::path::separator +
            "%NETWORK%" +
            elle::system::path::separator +
            "shelter") == elle::Status::Error)
        escape("unable to create the pattern");

      // create the immutable block pattern within the shelter.
      if (Lune::Network::Shelter::ImmutableBlock.Create(
            home +
            elle::system::path::separator +
            "networks" +
            elle::system::path::separator +
            "%NETWORK%" +
            elle::system::path::separator +
            "shelter" +
            elle::system::path::separator +
            "%ADDRESS%" +
            nucleus::proton::Block::Extension) == elle::Status::Error)
        escape("unable to create the pattern");

      // create the mutable block pattern within the shelter.
      if (Lune::Network::Shelter::MutableBlock.Create(
            home +
            elle::system::path::separator +
            "networks" +
            elle::system::path::separator +
            "%NETWORK%" +
            elle::system::path::separator +
            "shelter" +
            elle::system::path::separator +
            "%ADDRESS%" +
            "#" +
            "%REVISION%" +
            nucleus::proton::Block::Extension) == elle::Status::Error)
        escape("unable to create the pattern");

      // create the mutable block's history pattern within the shelter.
      if (Lune::Network::Shelter::History.Create(
            home +
            elle::system::path::separator +
            "networks" +
            elle::system::path::separator +
            "%NETWORK%" +
            elle::system::path::separator +
            "shelter" +
            elle::system::path::separator +
            "%ADDRESS%" +
            nucleus::proton::History::Extension) == elle::Status::Error)
        escape("unable to create the pattern");
    }

    return elle::Status::Ok;
  }

  ///
  /// this method cleans the static definitions.
  ///
  elle::Status          Lune::Clean()
  {
    // nothing to do.

    return elle::Status::Ok;
  }

}
