#ifndef ELLE_PASSPORT_HH
# define ELLE_PASSPORT_HH

# include <lune/fwd.hh>
# include <elle/Authority.hh>
# include <elle/concept/Fileable.hh>
# include <elle/concept/Uniquable.hh>
# include <elle/cryptography/Signature.hh>
# include <hole/Label.hh>

# include <elle/idiom/Open.hh>

# include <elle/Authority.hh>

namespace elle
{
  ///
  /// this class uniquely identify a device through a label which is
  /// used by the storage layer to locate the nodes responsible for a
  /// block's replica for instance.
  ///
  class Passport:
    public elle::concept::MakeFileable<Passport>,
    public elle::concept::MakeUniquable<Passport>
  {
    /*-------------.
    | Construction |
    `-------------*/
  public:
    Passport(); // XXX[to deserialize]
    Passport(hole::Label const& label,
             elle::String const& id);

  public:
    //
    // methods
    //
    elle::Status        Seal(elle::Authority const&);
    elle::Status        Validate(elle::Authority const&) const;

    //
    // interfaces
    //
  public:
    // dumpable
    elle::Status        Dump(const elle::Natural32 = 0) const;

    // fileable
    ELLE_CONCEPT_FILEABLE_METHODS();

    //
    // attributes
    //
    hole::Label         label;

    // XXX[temporary: mongodb id]
    elle::String        id;

    elle::cryptography::Signature     signature;
  };

}

# include <elle/Passport.hxx>

#endif
