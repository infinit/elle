#include <elle/Authority.hh>

#include <elle/serialize/TupleSerializer.hxx>
#include <elle/io/File.hh>

#include <cryptography/PublicKey.hh>
#include <cryptography/PrivateKey.hh>
#include <cryptography/KeyPair.hh>
#include <cryptography/Cipher.hh>
#include <cryptography/SecretKey.hh>

namespace elle
{
  /*-------------.
  | Construction |
  `-------------*/

  Authority::Authority(Authority const& from):
    type(from.type),
    K(from.K),
    k(nullptr),
    cipher(nullptr)
  {
    if (from.k)
      this->k = new cryptography::PrivateKey(*from.k);
    if (from.cipher)
      this->cipher = new cryptography::Cipher(*from.cipher);
  }

  Authority::Authority(cryptography::KeyPair const& pair):
    type(Authority::TypePair),
    K(pair.K()),
    k(new cryptography::PrivateKey(pair.k())),
    cipher(nullptr)
  {}

  Authority::Authority(cryptography::PublicKey const& K):
    type(Authority::TypePublic),
    K(K),
    k(0),
    cipher(nullptr)
  {}

  Authority::Authority(elle::io::Path const& path):
    k(0),
    cipher(0)
  {
    if (!elle::Authority::exists(path))
      throw reactor::Exception(
        elle::concurrency::scheduler(),
        elle::sprintf("unable to locate the authority file %s",
                      path));
    this->load(path);
  }

  Authority::~Authority()
  {
    delete this->k;
    delete this->cipher;
  }

//
// ---------- methods ---------------------------------------------------------
//

  /// this method encrypts the keys.
  ///
  elle::Status          Authority::Encrypt(const elle::String&          pass)
  {
    cryptography::SecretKey     key;

    // create a secret key with this pass.
    if (key.Create(pass) == elle::Status::Error)
      escape("unable to create the secret key");

    delete this->cipher;
    this->cipher = nullptr;

    // allocate the cipher.
    this->cipher = new cryptography::Cipher;

    // encrypt depending on the type.
    switch (this->type)
      {
      case Authority::TypePair:
        {
          std::tuple<
            cryptography::PublicKey const&,
            cryptography::PrivateKey const&
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
    cryptography::SecretKey     key;

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
          this->k = new cryptography::PrivateKey;

          // decrypt the authority.
          std::tuple<
            cryptography::PublicKey&,
            cryptography::PrivateKey&
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
}
