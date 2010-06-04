//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// file          /home/mycure/infinit/lune/Passport.hh
//
// created       julien quintard   [sat may  1 21:16:41 2010]
// updated       julien quintard   [fri may 28 17:32:05 2010]
//

#ifndef LUNE_PASSPORT_HH
#define LUNE_PASSPORT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <lune/Authority.hh>
#include <lune/Identity.hh>

#include <etoile/hole/Address.hh>

namespace lune
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class represents a passport linking a user to a universe.
  ///
  class Passport:
    public elle::Object
  {
  public:
    //
    // constants
    //
    static const elle::String		Extension;

    //
    // constructors & destructors
    //
    Passport();
    ~Passport();

    //
    // methods
    //
    elle::Status	Create(const elle::String&,
			       const elle::String&);

    elle::Status	Encrypt(const lune::Identity&);
    elle::Status	Decrypt(const lune::Identity&);

    elle::Status	Seal(const Authority&);
    elle::Status	Validate(const Authority&) const;

    elle::Status	Load(const elle::String&);
    elle::Status	Store(const elle::String&) const;

    elle::Status	Load(const elle::String&,
			     const elle::String&);
    elle::Status	Store(const elle::String&,
			      const elle::String&) const;

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

    //
    // attributes
    //
    elle::String		user;
    elle::String		universe;
    elle::KeyPair*		pair;
    elle::Code*			code;
    elle::Signature		signature;
  };

}

#endif
