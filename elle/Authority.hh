#ifndef  ELLE_AUTHORITY_HH
# define ELLE_AUTHORITY_HH

# include <elle/cryptography/fwd.hh>
# include <elle/cryptography/PublicKey.hh>
# include <elle/concept/Fileable.hh>
# include <elle/radix/Object.hh>

# include <elle/idiom/Open.hh>

namespace elle
{

  ///
  /// this class represents the authority.
  ///
  /// the authority can be represented through its whole key pair through
  /// which it can both sign and verify signatures or only through the
  /// public key, the most common case, through which it is only used to
  /// verify signatures.
  ///
  class Authority
    : public elle::radix::Object
    , public elle::concept::MakeFileable<Authority>
  {
  public:
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
    elle::Status        Create(elle::cryptography::KeyPair const&);
    elle::Status        Create(elle::cryptography::PublicKey const&);

    elle::Status        Encrypt(const elle::String&);
    elle::Status        Decrypt(const elle::String&);

    //
    // static methods
    //
  private:

    //
    // interfaces
    //
  public:
    // object
    declare(Authority);

    // dumpable
    elle::Status        Dump(const elle::Natural32 = 0) const;

    // fileable
    ELLE_CONCEPT_FILEABLE_METHODS();

    //
    // attributes
    //
    Type                type;

    elle::cryptography::PublicKey     K;
    elle::cryptography::PrivateKey*   k;

    elle::cryptography::Cipher*       cipher;
  };

}

#include <elle/Authority.hxx>

#endif
