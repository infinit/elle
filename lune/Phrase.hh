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
// updated       julien quintard   [wed may  5 00:08:31 2010]
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
  /// this class represents a phrase i.e a string linking applications
  /// with the user's agent.
  ///
  class Phrase:
    public elle::Object<>
  {
  public:
    //
    // methods
    //
    elle::Status	Create(const elle::String&);

    //
    // interfaces
    //

    // object
    declare(Phrase, _());

    // dumpable
    elle::Status	Dump(const elle::Natural32 = 0) const;

    // archivable
    elle::Status	Serialize(elle::Archive&) const;
    elle::Status	Extract(elle::Archive&);

    //
    // attributes
    //
    elle::String	string;
  };

}

#endif
