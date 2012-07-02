#ifndef LUNE_PASSPORT_HH
# define LUNE_PASSPORT_HH

# include <lune/fwd.hh>
# include <elle/radix/Object.hh>
# include <elle/concept/Fileable.hh>
# include <elle/concept/Uniquable.hh>
# include <elle/cryptography/Signature.hh>
# include <hole/Label.hh>

# include <elle/idiom/Open.hh>

namespace lune
{

  ///
  /// this class uniquely identify a device through a label which is
  /// used by the storage layer to locate the nodes responsible for a
  /// block's replica for instance.
  ///
  class Passport
    : public elle::radix::Object
    , public elle::concept::MakeFileable<Passport>
    , public elle::concept::MakeUniquable<Passport>
  {
  public:
    //
    // constants
    //
    static const elle::String           Extension;

    //
    // methods
    //
    elle::Status        Create(const hole::Label& label,
                               const elle::String& id);

    elle::Status        Seal(const Authority&);
    elle::Status        Validate(const Authority&) const;

    //
    // interfaces
    //

    // object
    declare(Passport);

    // dumpable
    elle::Status        Dump(const elle::Natural32 = 0) const;

    // fileable
    ELLE_CONCEPT_FILEABLE_METHODS();
    elle::Status        Load();
    elle::Status        Store() const;
    elle::Status        Erase() const;
    elle::Status        Exist() const;

    //
    // attributes
    //
    hole::Label         label;

    // XXX[temporary: mongodb id]
    elle::String        id;

    elle::cryptography::Signature     signature;
  };

}

# include <lune/Passport.hxx>

#endif
