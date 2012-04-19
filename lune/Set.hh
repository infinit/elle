#ifndef  LUNE_SET_HH
# define LUNE_SET_HH

# include <elle/types.hh>

# include <elle/network/Locus.hh>

# include <elle/idiom/Open.hh>

namespace lune
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class holds the set or subset of initial nodes required for
  /// a hole implementation to initiate connections.
  ///
  class Set:
    public elle::radix::Object,
    public elle::io::Fileable<Set>
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

    //
    // interfaces
    //

    // object
    declare(Set);

    // dumpable
    elle::Status        Dump(const elle::Natural32 = 0) const;

    // archivable
    //elle::Status        Serialize(elle::Archive&) const;
    //elle::Status        Extract(elle::Archive&);

    // fileable
    elle::Status        Load(const elle::String&);
    elle::Status        Store(const elle::String&) const;
    elle::Status        Erase(const elle::String&) const;
    elle::Status        Exist(const elle::String&) const;

    //
    // attributes
    //
    Container           loci;
  };

}

#endif
