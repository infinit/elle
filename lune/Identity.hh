//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// author        julien quintard   [wed may  5 20:55:12 2010]
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
  /// this class represents an identity issued by the Infinit authority
  /// which represents a user.
  ///
  /// note that the name attribute is supposed to be unique as it plays the
  /// role of identifier.
  ///
  class Identity:
    public elle::Object,
    public virtual elle::Fileable<elle::FormatCustom>
  {
  public:
    //
    // constants
    //
    static const elle::String		Extension;

    //
    // constructors & destructors
    //
    Identity();
    ~Identity();

    //
    // methods
    //
    elle::Status	Create(const elle::String&,
			       const elle::KeyPair&);

    elle::Status	Encrypt(const elle::String&);
    elle::Status	Decrypt(const elle::String&);

    elle::Status	Seal(const Authority&);
    elle::Status	Validate(const Authority&) const;

    //
    // interfaces
    //

    // object
    declare(Identity);

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

    elle::Status	Load(const elle::String&);
    elle::Status	Store(const elle::String&) const;
    elle::Status	Erase(const elle::String&) const;
    elle::Status	Exist(const elle::String&) const;

    //
    // attributes
    //
    elle::String	name;
    elle::KeyPair	pair;
    elle::Signature	signature;

    elle::Cipher*	cipher;
  };

}

#endif
