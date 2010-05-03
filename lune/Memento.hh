//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// file          /home/mycure/infinit/lune/Memento.hh
//
// created       julien quintard   [sat may  1 21:16:41 2010]
// updated       julien quintard   [mon may  3 12:23:09 2010]
//

#ifndef LUNE_MEMENTO_HH
#define LUNE_MEMENTO_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <etoile/Etoile.hh>

namespace lune
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class represents a universe descriptor.
  ///
  class Memento:
    public elle::Object<>
  {
  public:
    //
    // constants
    //
    static const elle::String		Section;

    //
    // methods
    //
    elle::Status	Create(const elle::String&,
			       const etoile::Address&);

    //
    // interfaces
    //

    // dumpable
    elle::Status	Dump(const elle::Natural32 = 0) const;

    // archivable
    elle::Status	Serialize(elle::Archive&) const;
    elle::Status	Extract(elle::Archive&);

    //
    // attributes
    //
    elle::Settings	settings;
  };

}

#endif
