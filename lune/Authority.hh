//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// file          /home/mycure/infinit/lune/Authority.hh
//
// created       julien quintard   [tue may  4 23:35:57 2010]
// updated       julien quintard   [wed may  5 21:43:19 2010]
//

#ifndef LUNE_AUTHORITY_HH
#define LUNE_AUTHORITY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace lune
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class represents the authority.
  ///
  /// the authority can be represented through its whole key pair in which
  /// can it can both sign and verify signatures or only through the
  /// public key, the most common case, in which case it is only used to
  /// verify signatures.
  ///
  class Authority:
    public elle::Object<elle::FormatBase64, elle::FormatCustom>
  {
  public:
    //
    // constants
    //
    static const elle::String		Extension;

    //
    // constructors & destructors
    //
    Authority();
    ~Authority();

    //
    // methods
    //
    elle::Status	Create(const elle::KeyPair&);
    elle::Status	Create(const elle::PublicKey&);

    //
    // interfaces
    //

    // object
    declare(Authority, _(elle::FormatBase64, elle::FormatCustom));

    // dumpable
    elle::Status	Dump(const elle::Natural32 = 0) const;

    // archivable
    elle::Status	Serialize(elle::Archive&) const;
    elle::Status	Extract(elle::Archive&);

    // fileable
    elle::Status	Load(const elle::String&);
    elle::Status	Store(const elle::String&) const;

    //
    // attributes
    //
    elle::PublicKey	K;
    elle::PrivateKey*	k;
  };

}

#endif
