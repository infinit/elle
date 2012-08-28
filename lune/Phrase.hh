#ifndef LUNE_PHRASE_HH
# define LUNE_PHRASE_HH

# include <elle/types.hh>
# include <elle/radix/Object.hh>
# include <elle/concept/Fileable.hh>
# include <elle/network/Port.hh>

# include <elle/idiom/Open.hh>

namespace lune
{

  ///
  /// this class represents a phrase i.e a string enabling applications
  /// run by the user having launched the software to communicate with
  /// Infinit and thus trigger additional functionalities.
  ///
  /// noteworthy is that a phrase is made specific to both a user and
  /// a network so that a single user can launch Infini twice or more, even
  /// with a different identity, without overwritting the phrase.
  ///
  /// the portal attribute represents the name of the local socket to
  /// connect to in order to issue requests to Infinit.
  ///
  class Phrase:
    public elle::radix::Object,
    public elle::concept::MakeFileable<Phrase>
  {
  public:
    //
    // constants
    //
    static const elle::String           Extension;

    //
    // methods
    //
    elle::Status        Create(const elle::network::Port,
                               const elle::String&);

  private:
    /// XXX
    static
    elle::io::Path
    _path(elle::String const& name);
    /// XXX
    static
    elle::io::Path
    _path(elle::String const& network,
          elle::String const& name);

    //
    // interfaces
    //
  public:
    // object
    declare(Phrase);
    elle::Boolean       operator==(const Phrase&) const;

    // dumpable
    elle::Status        Dump(const elle::Natural32 = 0) const;

    // fileable
    ELLE_CONCEPT_FILEABLE_METHODS();

    void
    load(elle::String const& name);
    void
    store(elle::String const& name) const;
    static
    void
    erase(elle::String const& name);
    static
    elle::Boolean
    exists(elle::String const& name);

    void
    load(elle::String const& network,
         elle::String const& name);
    void
    store(elle::String const& network,
          elle::String const& name) const;
    static
    void
    erase(elle::String const& network,
          elle::String const& name);
    static
    elle::Boolean
    exists(elle::String const& network,
           elle::String const& name);

    //
    // attributes
    //
    elle::network::Port port;
    elle::String pass;
  };

}

#include <lune/Phrase.hxx>

#endif
