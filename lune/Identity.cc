#include <lune/Identity.hh>
#include <elle/Authority.hh>
#include <lune/Lune.hh>

#include <common/common.hh>

#include <cryptography/KeyPair.hh>
#include <cryptography/Code.hh>
#include <cryptography/SecretKey.hh>
#include <elle/os/path.hh>
#include <elle/io/File.hh>
#include <elle/io/Piece.hh>
#include <elle/serialize/TupleSerializer.hxx>

namespace path = elle::os::path;

namespace lune
{

  ELLE_LOG_COMPONENT("infinit.lune.Identity");

//
// ---------- constructors & destructors --------------------------------------
//

  ///
  /// default constructor.
  ///
  Identity::Identity():
    _pair(nullptr),
    _signature(nullptr),
    code(nullptr)
  {
  }

  Identity::Identity(Identity const& other):
    _id(other._id),
    name(other.name),
    _pair(new cryptography::KeyPair{*other._pair}),
    _signature(new cryptography::Signature{*other._signature}),
    code(new cryptography::Code{*other.code})
  {
  }

  ///
  /// destructor.
  ///
  Identity::~Identity()
  {
    delete this->_signature;
    delete this->_pair;
    delete this->code;
  }

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method creates an identity based on the given key pair.
  ///
  elle::Status
  Identity::Create(elle::String const& user_id,
                   const elle::String& user_name,
                   cryptography::KeyPair const& pair)
  {
    this->_id = user_id;
    this->name = user_name;

    delete this->_pair;
    this->_pair = new cryptography::KeyPair{pair};

    return elle::Status::Ok;
  }

  ///
  /// this method encrypts the key pair.
  ///
  elle::Status          Identity::Encrypt(const elle::String&   pass)
  {
    // XXX[factor algo with decrypt()]
    cryptography::SecretKey key(cryptography::cipher::Algorithm::aes256, pass);

    ELLE_ASSERT(this->_pair != nullptr);

    // allocate the code.
    this->code = new cryptography::Code{
      key.encrypt(*this->_pair)};

    return elle::Status::Ok;
  }

  ///
  /// this method decrypts the key pair.
  ///
  elle::Status          Identity::Decrypt(const elle::String&   pass)
  {
    // check the code.
    if (this->code == nullptr)
      throw elle::Exception("unable to decrypt an unencrypted identity");

    cryptography::SecretKey key{cryptography::cipher::Algorithm::aes256, pass};

    // decrypt the authority.
    delete this->_pair;
    this->_pair = new cryptography::KeyPair{
      key.decrypt<cryptography::KeyPair>(*this->code)};

    return elle::Status::Ok;
  }

  ///
  /// this method clears the identity i.e removes the code.
  ///
  /// this is required for the Serialize() method to consider the identity
  /// in its unencrypted form.
  ///
  elle::Status          Identity::Clear()
  {
    if (this->code != nullptr)
      {
        delete this->code;

        this->code = nullptr;
      }

    return elle::Status::Ok;
  }

  ///
  /// this method seals the identity with the authority.
  ///
  elle::Status
  Identity::Seal(elle::Authority const& authority)
  {
    // check the code.
    if (this->code == nullptr)
      throw elle::Exception("unable to seal an unencrypted identity");

    // sign with the authority.
    delete this->_signature;
    this->_signature = nullptr;
    this->_signature = new cryptography::Signature{
      authority.k->sign(
        elle::serialize::make_tuple(this->_id, this->name, *this->code))};

    return elle::Status::Ok;
  }

  ///
  /// this method verifies the validity of the identity.
  ///
  elle::Status
  Identity::Validate(elle::Authority const& authority)
    const
  {
    // check the code.
    if (this->code == nullptr)
      throw elle::Exception("unable to verify an unencrypted identity");

    // verify the signature.
    ELLE_ASSERT(this->_signature != nullptr);

    if (authority.K().verify(
          *this->_signature,
          elle::serialize::make_tuple(this->_id,
                                      this->name,
                                      *this->code)) == false)
      throw elle::Exception("unable to verify the signature");

    return elle::Status::Ok;
  }

  std::string
  Identity::_path(elle::String const& user_id)
  {
    return path::join(
      common::infinit::user_directory(user_id),
      user_id + ".idy"
    );
  }

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
    if (this->_pair != nullptr)
      {
        std::cout << alignment << elle::io::Dumpable::Shift
                  << "[Pair] " << this->_pair << std::endl;
      }

    // dump the signature.
    if (this->_signature != nullptr)
      {
        std::cout << alignment << elle::io::Dumpable::Shift
                  << "[Signature] " << *this->_signature << std::endl;
      }

    // dump the code.
    if (this->code != nullptr)
      {
        std::cout << alignment << elle::io::Dumpable::Shift
                  << "[Code] " << this->code << std::endl;
      }

    return elle::Status::Ok;
  }

//
// ---------- fileable --------------------------------------------------------
//

  void
  Identity::load(elle::String const& user_id)
  {
    this->load(elle::io::Path{Identity::_path(user_id)});
  }

  void
  Identity::store() const
  {
    ELLE_TRACE("store identity %s", *this);
    this->store(elle::io::Path{Identity::_path(this->_id)});
  }

  void
  Identity::erase(elle::String const& user_id)
  {
    elle::concept::Fileable<>::erase(elle::io::Path{Identity::_path(user_id)});
  }

  elle::Boolean
  Identity::exists(elle::String const& user_id)
  {
    return (elle::os::path::exists(Identity::_path(user_id)));
  }

}
