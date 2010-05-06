//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// file          /home/mycure/infinit/lune/Identity.hh
//
// created       julien quintard   [wed may  5 20:55:12 2010]
// updated       julien quintard   [thu may  6 01:08:25 2010]
//

#ifndef LUNE_IDENTITY_HH
#define LUNE_IDENTITY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <lune/Authority.hh>

namespace lune
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class represents a identity, issued by the Infinit authority,
  /// which represents a user.
  ///
  class Identity:
    public elle::Object<elle::FormatBase64, elle::FormatCustom>
  {
  public:
    //
    // constants
    //
    static const elle::String		Extension;

    //
    // methods
    //
    elle::Status	Create(const elle::String&,
			       const elle::KeyPair&);

    elle::Status	Seal(const Authority&);
    elle::Status	Validate(const Authority&) const;

    //
    // interfaces
    //

    // object
    declare(Identity, _(elle::FormatBase64, elle::FormatCustom));

    // dumpable
    elle::Status	Dump(const elle::Natural32 = 0) const;

    // archivable
    elle::Status	Serialize(elle::Archive&) const;
    elle::Status	Extract(elle::Archive&);

    // fileable
    elle::Status	Load(const elle::String&,
			     const elle::String&);
    elle::Status	Store(const elle::String&,
			      const elle::String&) const;

    //
    // attributes
    //
    elle::String	name;
    elle::KeyPair	pair;

    elle::Signature	signature;
  };

}

#endif
