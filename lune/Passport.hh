//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// file          /home/mycure/infinit/lune/Passport.hh
//
// created       julien quintard   [thu aug 11 13:13:51 2011]
// updated       julien quintard   [thu aug 11 16:05:22 2011]
//

#ifndef LUNE_PASSPORT_HH
#define LUNE_PASSPORT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <lune/Authority.hh>

#include <hole/Label.hh>

namespace lune
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class uniquely identify a device.
  ///
  class Passport:
    public elle::Object,
    public virtual elle::Fileable<elle::FormatCustom>
  {
  public:
    //
    // constants
    //
    static const elle::String		Extension;

    //
    // methods
    //
    elle::Status	Create(const hole::Label&);

    elle::Status	Seal(const Authority&);
    elle::Status	Validate(const Authority&) const;

    //
    // interfaces
    //

    // object
    declare(Passport);

    // dumpable
    elle::Status	Dump(const elle::Natural32 = 0) const;

    // archivable
    elle::Status	Serialize(elle::Archive&) const;
    elle::Status	Extract(elle::Archive&);

    // fileable
    elle::Status	Load();
    elle::Status	Store() const;
    elle::Status	Erase() const;
    elle::Status	Exist() const;

    //
    // attributes
    //
    hole::Label		label;
    elle::Signature	signature;
  };

}

#endif
