//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// author        julien quintard   [sat may  1 12:52:01 2010]
//

#ifndef LUNE_PHRASE_HH
#define LUNE_PHRASE_HH

//
// ---------- includes --------------------------------------------------------
//

# include <elle/radix/Object.hh>
# include <elle/io/Fileable.hh>

namespace lune
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class represents a phrase i.e a string enabling applications
  /// run by the user having launched the software to communicate with
  /// Infinit and thus trigger additional functionalities.
  ///
  /// noteworthy is that a phrase is made specific to both a user and
  /// a network so that a single user can launch Infini twice or more, even
  /// with a different identity, without overwritting the phrase.
  ///
  /// the portal attribute represents the name of the local socket to
  /// connect to in order to issue requests to Infinit.
  ///
  class Phrase:
    public elle::radix::Object,
    public elle::io::Fileable<Phrase>
  {
  public:
    //
    // constants
    //
    static const elle::String           Extension;

    //
    // methods
    //
    elle::Status        Create(const elle::String&,
                               const elle::String&);

    //
    // interfaces
    //

    // object
    declare(Phrase);
    elle::Boolean       operator==(const Phrase&) const;

    // dumpable
    elle::Status        Dump(const elle::Natural32 = 0) const;

    // archivable
    //elle::Status        Serialize(elle::Archive&) const;
    //elle::Status        Extract(elle::Archive&);

    // fileable
    elle::Status        Load(const elle::String&);
    elle::Status        Store(const elle::String&) const;
    elle::Status        Erase(const elle::String&) const;
    elle::Status        Exist(const elle::String&) const;

    //
    // attributes
    //
    elle::String        pass;
    elle::String        portal;
  };

}

#endif
