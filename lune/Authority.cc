#include <elle/cryptography/PublicKey.hh>
#include <elle/cryptography/PrivateKey.hh>
#include <elle/cryptography/KeyPair.hh>
#include <elle/cryptography/Cipher.hh>
#include <elle/cryptography/SecretKey.hh>
#include <elle/serialize/TupleSerializer.hxx>
#include <elle/standalone/Log.hh>
#include <elle/io/File.hh>

#include <lune/Authority.hh>
#include <lune/Lune.hh>

namespace lune
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this string defines the authority files extension.
  ///
  const elle::String            Authority::Extension = ".auth";

//
// ---------- constructors & destructors --------------------------------------
//

  ///
  /// default constructor.
  ///
  Authority::Authority():
    k(nullptr),
    cipher(nullptr)
  {
  }

  ///
  /// destructor.
  ///
  Authority::~Authority()
  {
    // release the private key, if present.
    if (this->k != nullptr)
      delete this->k;

    // release the cipher.
    if (this->cipher != nullptr)
      delete this->cipher;
  }

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method creates an authority based on the given key pair.
  ///
  elle::Status          Authority::Create(elle::cryptography::KeyPair const&  pair)
  {
    // set the type.
    this->type = Authority::TypePair;

    // set the public key.
    this->K = pair.K;

    // allocate the private key.
    this->k = new elle::cryptography::PrivateKey(pair.k);

    return elle::Status::Ok;
  }

  ///
  /// this method creates an authority based on the given public key only.
  ///
  elle::Status          Authority::Create(elle::cryptography::PublicKey const& K)
  {
    // set the type.
    this->type = Authority::TypePublic;

    // set the public key.
    this->K = K;

    return elle::Status::Ok;
  }

  ///
  /// this method encrypts the keys.
  ///
  elle::Status          Authority::Encrypt(const elle::String&          pass)
  {
    elle::cryptography::SecretKey     key;

    // create a secret key with this pass.
    if (key.Create(pass) == elle::Status::Error)
      escape("unable to create the secret key");

    delete this->cipher;
    this->cipher = nullptr;

    // allocate the cipher.
    this->cipher = new elle::cryptography::Cipher;

    // encrypt depending on the type.
    switch (this->type)
      {
      case Authority::TypePair:
        {
          std::tuple<
            elle::cryptography::PublicKey const&,
            elle::cryptography::PrivateKey const&
          >                                     res{this->K, *this->k};
          if (key.Encrypt(res, *this->cipher) == elle::Status::Error)
            escape("unable to encrypt the authority");

          break;
        }
      case Authority::TypePublic:
        {
          // encrypt the authority.
          if (key.Encrypt(this->K,
                          *this->cipher) == elle::Status::Error)
            escape("unable to encrypt the authority");

          break;
        }
      case Authority::TypeUnknown:
        escape("unknown type");
      default:
        escape("invalid type");
      }

    return elle::Status::Ok;
  }

  ///
  /// this method decrypts the keys.
  ///
  elle::Status          Authority::Decrypt(const elle::String&          pass)
  {
    elle::cryptography::SecretKey     key;

    // check the cipher.
    if (this->cipher == nullptr)
      escape("unable to decrypt an unencrypted authority");

    // create a secret key with this pass.
    if (key.Create(pass) == elle::Status::Error)
      escape("unable to create the secret key");

    // decrypt depending on the type.
    switch (this->type)
      {
      case Authority::TypePair:
        {
          // allocate the private key.
          this->k = new elle::cryptography::PrivateKey;

          // decrypt the authority.
          std::tuple<
            elle::cryptography::PublicKey&,
            elle::cryptography::PrivateKey&
          >                                     res{this->K, *this->k};

          if (key.Decrypt(*this->cipher, res) == elle::Status::Error)
            escape("unable to decrypt the authority");

          break;
        }
      case Authority::TypePublic:
        {
          // decrypt the authority.
          if (key.Decrypt(*this->cipher,
                          this->K) == elle::Status::Error)
            escape("unable to decrypt the authority");

          break;
        }
      case Authority::TypeUnknown:
        escape("unknown type");
      default:
        escape("invalid type");
      }

    return elle::Status::Ok;
  }

  elle::io::Path
  Authority::_path()
  {
    return (elle::io::Path(Lune::Authority));
  }

//
// ---------- object ----------------------------------------------------------
//

  ///
  /// this macro-function call generates the object.
  ///
  embed(Authority, _());

//
// ---------- dumpable --------------------------------------------------------
//

  ///
  /// this method dumps a authority.
  ///
  elle::Status          Authority::Dump(const elle::Natural32   margin) const
  {
    elle::String        alignment(margin, ' ');
    elle::String        unique;

    std::cout << alignment << "[Authority]" << std::endl;

    // dump the type.
    std::cout << alignment << elle::io::Dumpable::Shift
              << "[Type] " << this->type << std::endl;

    // dump the public key.
    if (this->K.Dump(margin + 2) == elle::Status::Error)
      escape("unable to dump the public key");

    // if present...
    if (this->k != nullptr)
      {
        // ...dump the private key.
        if (this->k->Dump(margin + 2) == elle::Status::Error)
          escape("unable to dump the private key");
      }

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
  Authority::load()
  {
    this->load(Authority::_path());
  }

  void
  Authority::store() const
  {
    this->store(Authority::_path());
  }

  void
  Authority::erase()
  {
    elle::concept::Fileable<>::erase(Authority::_path());
  }

  elle::Boolean
  Authority::exists()
  {
    return (elle::concept::Fileable<>::exists(Authority::_path()));
  }

}
