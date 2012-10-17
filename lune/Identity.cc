#include <lune/Identity.hh>
#include <elle/Authority.hh>
#include <lune/Lune.hh>

#include <elle/cryptography/KeyPair.hh>
#include <elle/cryptography/Cipher.hh>
#include <elle/cryptography/SecretKey.hh>
#include <elle/io/File.hh>
#include <elle/io/Piece.hh>
#include <elle/serialize/TupleSerializer.hxx>

namespace lune
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this string defines the identity files extension.
  ///
  const elle::String            Identity::Extension = ".idy";

//
// ---------- constructors & destructors --------------------------------------
//

  ///
  /// default constructor.
  ///
  Identity::Identity():
    pair(elle::cryptography::KeyPair::Null),
    cipher(nullptr)
  {
  }

  ///
  /// destructor.
  ///
  Identity::~Identity()
  {
    // release the cipher.
    if (this->cipher != nullptr)
      delete this->cipher;
  }

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method creates an identity based on the given key pair.
  ///
  elle::Status          Identity::Create(elle::String const&    id,
                                         const elle::String&    name,
                                         elle::cryptography::KeyPair const&   pair)
  {
    // One does not simply ...
    assert(pair.k.key() != nullptr);
    assert(pair.K.key() != nullptr);

    this->_id = id;
    this->name = name;
    this->pair = pair;

    assert(this->pair.k.key() != nullptr);
    assert(this->pair.K.key() != nullptr);

    return elle::Status::Ok;
  }

  ///
  /// this method encrypts the key pair.
  ///
  elle::Status          Identity::Encrypt(const elle::String&   pass)
  {
    elle::cryptography::SecretKey     key;

    // create a secret key with this pass.
    if (key.Create(pass) == elle::Status::Error)
      escape("unable to create the secret key");

    // allocate the cipher.
    this->cipher = new elle::cryptography::Cipher;

    // encrypt the authority.
    if (key.Encrypt(this->pair,
                    *this->cipher) == elle::Status::Error)
      escape("unable to encrypt the key pair");

    return elle::Status::Ok;
  }

  ///
  /// this method decrypts the key pair.
  ///
  elle::Status          Identity::Decrypt(const elle::String&   pass)
  {
    elle::cryptography::SecretKey     key;

    // check the cipher.
    if (this->cipher == nullptr)
      escape("unable to decrypt an unencrypted identity");

    // create a secret key with this pass.
    if (key.Create(pass) == elle::Status::Error)
      escape("unable to create the secret key");

    // decrypt the authority.
    if (key.Decrypt(*this->cipher,
                    this->pair) == elle::Status::Error)
      escape("unable to decrypt the key pair");

    return elle::Status::Ok;
  }

  ///
  /// this method clears the identity i.e removes the cipher.
  ///
  /// this is required for the Serialize() method to consider the identity
  /// in its unencrypted form.
  ///
  elle::Status          Identity::Clear()
  {
    if (this->cipher != nullptr)
      {
        delete this->cipher;

        this->cipher = nullptr;
      }

    return elle::Status::Ok;
  }

  ///
  /// this method seals the identity with the authority.
  ///
  elle::Status
  Identity::Seal(elle::Authority const& authority)
  {
    // check the cipher.
    if (this->cipher == nullptr)
      escape("unable to seal an unencrypted identity");

    // sign the pair with the authority.
    this->signature =
      authority.k->sign(
        elle::serialize::make_tuple(this->_id, this->name, *this->cipher));

    return elle::Status::Ok;
  }

  ///
  /// this method verifies the validity of the identity.
  ///
  elle::Status
  Identity::Validate(elle::Authority const& authority)
    const
  {
    // check the cipher.
    if (this->cipher == nullptr)
      escape("unable to verify an unencrypted identity");

    // verify the signature.
    if (authority.K.Verify(this->signature,
                           elle::serialize::make_tuple(this->_id,
                                                       this->name,
                                                       *this->cipher)) == elle::Status::Error)
      escape("unable to verify the signature");

    return elle::Status::Ok;
  }

  elle::io::Path
  Identity::_path(elle::String const& user)
  {
    return (elle::io::Path(Lune::User::Identity,
                           elle::io::Piece("%USER%", user)));
  }

//
// ---------- object ----------------------------------------------------------
//

  ///
  /// this macro-function call generates the object.
  ///
  embed(Identity, _());

//
// ---------- dumpable --------------------------------------------------------
//

  ///
  /// this method dumps a identity.
  ///
  elle::Status          Identity::Dump(const elle::Natural32    margin) const
  {
    elle::String        alignment(margin, ' ');

    std::cout << alignment << "[Identity]" << std::endl;

    // dump the id.
    std::cout << alignment << elle::io::Dumpable::Shift
              << "[Id] " << this->_id << std::endl;

    // dump the name.
    std::cout << alignment << elle::io::Dumpable::Shift
              << "[Name] " << this->name << std::endl;

    // dump the pair.
    if (this->pair.Dump(margin + 2) == elle::Status::Error)
      escape("unable to dump the pair");

    // dump the signature.
    if (this->signature.Dump(margin + 2) == elle::Status::Error)
      escape("unable to dump the signature");

    // dump the cipher.
    if (this->cipher != nullptr)
      {
        if (this->cipher->Dump(margin + 2) == elle::Status::Error)
          escape("unable to dump the cipher");
      }

    return elle::Status::Ok;
  }

//
// ---------- fileable --------------------------------------------------------
//

  void
  Identity::load(elle::String const& user)
  {
    this->load(Identity::_path(user));
  }

  void
  Identity::store(elle::String const& user) const
  {
    this->store(Identity::_path(user));
  }

  void
  Identity::erase(elle::String const& user)
  {
    elle::concept::Fileable<>::erase(Identity::_path(user));
  }

  elle::Boolean
  Identity::exists(elle::String const& user)
  {
    return (elle::concept::Fileable<>::exists(Identity::_path(user)));
  }

}
