//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// file          /home/mycure/infinit/lune/Dictionary.hh
//
// created       julien quintard   [mon may 10 15:52:06 2010]
// updated       julien quintard   [thu apr 28 16:09:52 2011]
//

#ifndef LUNE_DICTIONARY_HH
#define LUNE_DICTIONARY_HH

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
  class Dictionary:
    public elle::Object,
    public virtual elle::Fileable<elle::FormatCustom>
  {
  public:
    //
    // constants
    //
    static const elle::String		Extension;

    //
    // interfaces
    //

    // object
    declare(Dictionary);

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
    Map<elle::PublicKey>	users;
    Map<etoile::hole::Address>	groups;
  };

}

#endif
