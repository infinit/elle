//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// file          /home/mycure/infinit/lune/Phrase.hh
//
// created       julien quintard   [sat may  1 12:52:01 2010]
// updated       julien quintard   [tue sep  6 22:30:29 2011]
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

    // dumpable
    elle::Status	Dump(const elle::Natural32 = 0) const;

    // archivable
    elle::Status	Serialize(elle::Archive&) const;
    elle::Status	Extract(elle::Archive&);

    // fileable
    elle::Status	Load(const elle::String&);
    elle::Status	Store(const elle::String&) const;
    elle::Status	Erase(const elle::String&) const;
    elle::Status	Exist(const elle::String&) const;

    //
    // attributes
    //
    elle::String	string;
  };

}

#endif
