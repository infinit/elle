#ifndef LUNE_PASSPORT_HH
# define LUNE_PASSPORT_HH

# include <lune/Authority.hh>

# include <hole/Label.hh>

# include <elle/idiom/Open.hh>

namespace lune
{

  ///
  /// this class uniquely identify a device through a label which is
  /// used by the storage layer to locate the nodes responsible for a
  /// block's replica for instance.
  ///
  class Passport:
    public elle::radix::Object,
    public elle::io::Fileable<Passport>
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

    // archivable
    //elle::Status        Serialize(elle::Archive&) const;
    //elle::Status        Extract(elle::Archive&);

    // fileable
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

# include <elle/idiom/Close.hh>

#endif
