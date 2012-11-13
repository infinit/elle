#ifndef  ELLE_AUTHORITY_HH
# define ELLE_AUTHORITY_HH

# include <elle/concept/Fileable.hh>
# include <elle/radix/Object.hh>

# include <elle/idiom/Open.hh>

# include <cryptography/fwd.hh>
# include <cryptography/PublicKey.hh>

// XXX[temporary: for cryptography]
using namespace infinit;

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
  class Authority:
    public elle::concept::MakeFileable<Authority>
  {
  public:
    enum Type
      {
        TypeUnknown,
        TypePair,
        TypePublic
      };

  /*-------------.
  | Construction |
  `-------------*/
  public:
    /// Create an authority based on the given key pair.
    Authority(cryptography::KeyPair const&);
    /// Create an authority based on the given public key only.
    Authority(cryptography::PublicKey const&);
    /// Create a copy of an Authority.
    Authority(Authority const& from);
    /// Deserialize an Authority.
    Authority(elle::io::Path const& path);
    /// Dispose of an Authority.
    ~Authority();

    //
    // methods
    //

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
    // dumpable
    elle::Status        Dump(const elle::Natural32 = 0) const;

    // fileable
    ELLE_CONCEPT_FILEABLE_METHODS();

    //
    // attributes
    //
    Type                type;

    cryptography::PublicKey     K;
    cryptography::PrivateKey*   k;

    cryptography::Cipher*       cipher;
  };

}

#include <elle/Authority.hxx>

#endif
