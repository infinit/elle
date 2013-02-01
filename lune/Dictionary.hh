#ifndef LUNE_DICTIONARY_HH
# define LUNE_DICTIONARY_HH

# include <elle/operator.hh>
# include <elle/concept/Fileable.hh>

# include <cryptography/PublicKey.hh>
// XXX[temporary: for cryptography]
using namespace infinit;

# include <nucleus/proton/Address.hh>

# include <lune/Map.hh>

# include <boost/noncopyable.hpp>

namespace lune
{

  ///
  /// this class maintains a mapping between local users/groups and
  /// Infinit identifiers so that Infinit can be used with local file
  /// system entities such as UNIX's UIDs/GIDs for instance.
  ///
  class Dictionary:
    public elle::concept::MakeFileable<Dictionary>,
    private boost::noncopyable
  {
  public:
    //
    // methods
    //
  private:
    /// XXX
    static
    elle::io::Path
    _path(elle::String const& user_id);

    //
    // interfaces
    //
  public:
    ELLE_OPERATOR_NO_ASSIGNMENT(Dictionary);

    // dumpable
    elle::Status        Dump(const elle::Natural32 = 0) const;

    // fileable
    ELLE_CONCEPT_FILEABLE_METHODS();

    void
    load(elle::String const& user_id);
    void
    store(elle::String const& user_id) const;
    static
    void
    erase(elle::String const& user_id);
    static
    elle::Boolean
    exists(elle::String const& user_id);

    //
    // attributes
    //
    Map<cryptography::PublicKey>        users;
    Map<nucleus::proton::Address> groups;
  };

}

#include <lune/Dictionary.hxx>

#endif
