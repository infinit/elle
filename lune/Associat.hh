//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// file          /home/mycure/infinit/lune/Associat.hh
//
// created       julien quintard   [mon may 10 15:52:06 2010]
// updated       julien quintard   [fri may 28 12:30:39 2010]
//

#ifndef LUNE_ASSOCIAT_HH
#define LUNE_ASSOCIAT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/hole/Address.hh>

#include <lune/Map.hh>
#include <lune/Lune.hh>

namespace lune
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class contains the links between local users and groups and
  /// Infinit identifiers so that Infinit can be used with local file
  /// system entities such as UNIX's UIDs/GIDs for instance.
  ///
  class Associat:
    public elle::Object
  {
  public:
    //
    // constants
    //
    static const elle::String		Extension;

    //
    // methods
    //
    elle::Status	Load();
    elle::Status	Store() const;

    //
    // interfaces
    //

    // object
    declare(Associat);

    // dumpable
    elle::Status	Dump(const elle::Natural32 = 0) const;

    // archivable
    elle::Status	Serialize(elle::Archive&) const;
    elle::Status	Extract(elle::Archive&);

    //
    // attributes
    //
    Map<elle::PublicKey>	users;
    Map<etoile::hole::Address>	groups;
  };

}

#endif
