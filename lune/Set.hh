#ifndef  LUNE_SET_HH
# define LUNE_SET_HH

# include <elle/types.hh>
# include <elle/radix/Object.hh>
# include <elle/concept/Fileable.hh>

# include <elle/network/fwd.hh>

# include <elle/idiom/Open.hh>

namespace lune
{

  ///
  /// this class holds the set or subset of initial nodes required for
  /// a hole implementation to initiate connections.
  ///
  class Set:
    public elle::radix::Object,
    public elle::concept::MakeFileable<Set>
  {
  public:
    //
    // constants
    //
    static const elle::String           Extension;

    //
    // types
    //
    // XXX use std::set<elle::network::Locus> !
    typedef std::vector<elle::network::Locus>    Container;

    //
    // methods
    //
    elle::Status        Add(const elle::network::Locus&);
    elle::Status        Remove(const elle::network::Locus&);

  private:
    /// XXX
    static
    elle::io::Path
    _path(elle::String const& network);

    //
    // interfaces
    //
  public:
    // object
    declare(Set);

    // dumpable
    elle::Status        Dump(const elle::Natural32 = 0) const;

    // fileable
    ELLE_CONCEPT_FILEABLE_METHODS();

    void
    load(elle::String const& network);
    void
    store(elle::String const& network) const;
    static
    void
    erase(elle::String const& network);
    static
    elle::Boolean
    exists(elle::String const& network);

    //
    // attributes
    //
    Container           loci;
  };

}

#include <lune/Set.hxx>

#endif
