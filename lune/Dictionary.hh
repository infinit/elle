//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// author        julien quintard   [mon may 10 15:52:06 2010]
//

#ifndef LUNE_DICTIONARY_HH
#define LUNE_DICTIONARY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>
#include <nucleus/Nucleus.hh>

#include <lune/Map.hh>

namespace lune
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class maintains a mapping between local users/groups and
  /// Infinit identifiers so that Infinit can be used with local file
  /// system entities such as UNIX's UIDs/GIDs for instance.
  ///
  class Dictionary:
    public elle::radix::Object,
    public virtual elle::io::Fileable<Dictionary>
  {
  public:
    //
    // constants
    //
    static const elle::String           Extension;

    //
    // interfaces
    //

    // object
    declare(Dictionary);

    // dumpable
    elle::Status        Dump(const elle::Natural32 = 0) const;

    // archivable
    //elle::Status        Serialize(elle::Archive&) const;
    //elle::Status        Extract(elle::Archive&);

    // fileable
    elle::Status        Load();
    elle::Status        Store() const;
    elle::Status        Erase() const;
    elle::Status        Exist() const;

    elle::Status        Load(const elle::String&);
    elle::Status        Store(const elle::String&) const;
    elle::Status        Erase(const elle::String&) const;
    elle::Status        Exist(const elle::String&) const;

    //
    // attributes
    //
    Map<elle::cryptography::PublicKey>        users;
    Map<nucleus::Address>       groups;
  };

}

#endif
