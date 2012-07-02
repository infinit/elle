#ifndef  LUNE_DICTIONARY_HH
# define LUNE_DICTIONARY_HH

# include <elle/concept/Fileable.hh>
# include <nucleus/proton/Address.hh>

# include <lune/Map.hh>

# include <elle/idiom/Open.hh>

namespace lune
{

  ///
  /// this class maintains a mapping between local users/groups and
  /// Infinit identifiers so that Infinit can be used with local file
  /// system entities such as UNIX's UIDs/GIDs for instance.
  ///
  class Dictionary
    : public elle::radix::Object
    , public elle::concept::MakeFileable<Dictionary>
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

    // fileable
    ELLE_CONCEPT_FILEABLE_METHODS();
    elle::Status        Load(const elle::String&);
    elle::Status        Store(const elle::String&) const;
    elle::Status        Erase(const elle::String&) const;
    elle::Status        Exist(const elle::String&) const;

    //
    // attributes
    //
    Map<elle::cryptography::PublicKey>        users;
    Map<nucleus::proton::Address> groups;
  };

}

#include <lune/Dictionary.hxx>

#endif
