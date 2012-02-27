//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// author        julien quintard   [tue may  4 23:35:57 2010]
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
  /// the authority can be represented through its whole key pair through
  /// which it can both sign and verify signatures or only through the
  /// public key, the most common case, through which it is only used to
  /// verify signatures.
  ///
  class Authority:
    public elle::Object,
    public virtual elle::Fileable<elle::FormatCustom>
  {
  public:
    //
    // constants
    //
    static const elle::String           Extension;

    //
    // enumerations
    //
    enum Type
      {
        TypeUnknown,
        TypePair,
        TypePublic
      };

    //
    // constructors & destructors
    //
    Authority();
    ~Authority();

    //
    // methods
    //
    elle::Status        Create(const elle::KeyPair&);
    elle::Status        Create(const elle::PublicKey&);

    elle::Status        Encrypt(const elle::String&);
    elle::Status        Decrypt(const elle::String&);

    //
    // interfaces
    //

    // object
    declare(Authority);

    // dumpable
    elle::Status        Dump(const elle::Natural32 = 0) const;

    // archivable
    elle::Status        Serialize(elle::Archive&) const;
    elle::Status        Extract(elle::Archive&);

    // fileable
    elle::Status        Load();
    elle::Status        Load(elle::Path const& path);
    elle::Status        Store() const;
    elle::Status        Erase() const;
    elle::Status        Exist() const;

    //
    // attributes
    //
    Type                type;

    elle::PublicKey     K;
    elle::PrivateKey*   k;

    elle::Cipher*       cipher;
  };

}

#endif
