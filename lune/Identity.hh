#ifndef LUNE_IDENTITY_HH
# define LUNE_IDENTITY_HH

# include <elle/cryptography.fwd.hh>
# include <elle/cryptography/KeyPair.hh>
# include <elle/cryptography/Signature.hh>

# include <elle/concept/Fileable.hh>
# include <elle/radix/Object.hh>
# include <elle/concept/Uniquable.hh>

# include <elle/idiom/Open.hh>

namespace lune
{

  class Authority;

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
  class Identity
    : public elle::radix::Object
    , public elle::concept::MakeFileable<Identity>
    , public elle::concept::MakeUniquable<Identity>
  {
  public:
    static const elle::String           Extension;

    //
    // enumerations
    //
    enum Mode
      {
        ModeEncrypted,
        ModeUnencrypted
      };

    //
    // constructors & destructors
    //
    Identity();
    ~Identity();

    //
    // methods
    //
    elle::Status        Create(elle::String const&,
                               const elle::String&,
                               elle::cryptography::KeyPair const&);

    elle::Status        Encrypt(const elle::String&);
    elle::Status        Decrypt(const elle::String&);

    elle::Status        Clear();

    elle::Status        Seal(const Authority&);
    elle::Status        Validate(const Authority&) const;

    //
    // interfaces
    //

    // object
    declare(Identity);

    // dumpable
    elle::Status        Dump(const elle::Natural32 = 0) const;

    // archivable
    //elle::Status        Serialize(elle::Archive&) const;
    //elle::Status        Extract(elle::Archive&);

    // fileable
    ELLE_CONCEPT_FILEABLE_METHODS();
    elle::Status        Load();
    elle::Status        Store() const;
    elle::Status        Erase() const;
    elle::Status        Exist() const;

    elle::Status        Load(const elle::String&);
    elle::Status        Store(const elle::String&) const;
    elle::Status        Erase(const elle::String&) const;
    elle::Status        Exist(const elle::String&) const;

    //
    // attributes
    //
  private:
    elle::String        _id;
  public: // XXX
    elle::String                      name;
    elle::cryptography::KeyPair       pair;
    elle::cryptography::Signature     signature;

    elle::cryptography::Cipher*       cipher;

    ELLE_SERIALIZE_FRIEND_FOR(Identity);
  };

}

# include <elle/idiom/Close.hh>

#endif
#include <lune/Identity.hxx>
