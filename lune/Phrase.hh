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

#include <elle/Elle.hh>

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
  class Phrase:
    public elle::Object,
    public virtual elle::Fileable<elle::FormatCustom>
  {
  public:
    //
    // constants
    //
    static const elle::String		Extension;

    //
    // methods
    //
    elle::Status	Create(const elle::String&);

    //
    // interfaces
    //

    // object
    declare(Phrase);
    elle::Boolean	operator==(const Phrase&) const;

    // dumpable
    elle::Status	Dump(const elle::Natural32 = 0) const;

    // archivable
    elle::Status	Serialize(elle::Archive&) const;
    elle::Status	Extract(elle::Archive&);

    // fileable
    elle::Status	Load(const elle::String&,
			     const elle::String&);
    elle::Status	Store(const elle::String&,
			      const elle::String&) const;
    elle::Status	Erase(const elle::String&,
			      const elle::String&) const;
    elle::Status	Exist(const elle::String&,
			      const elle::String&) const;

    //
    // attributes
    //
    elle::String	string;
  };

}

#endif
