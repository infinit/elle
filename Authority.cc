#include <elle/Authority.hh>
#include <elle/log.hh>
#include <elle/serialize/TupleSerializer.hxx>
#include <elle/io/File.hh>

#include <cryptography/PublicKey.hh>
#include <cryptography/PrivateKey.hh>
#include <cryptography/KeyPair.hh>
#include <cryptography/Cipher.hh>
#include <cryptography/SecretKey.hh>

ELLE_LOG_COMPONENT("infinit.lune.Authority")

namespace elle
{
  /*-------------.
  | Construction |
  `-------------*/

  Authority::Authority(Authority const& from):
    type(from.type),
    _K(from._K),
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
    _K(pair.K()),
    k(new cryptography::PrivateKey{pair.k()}),
    cipher(nullptr)
  {}

  Authority::Authority(cryptography::PublicKey const& K):
    type(Authority::TypePublic),
    _K(K),
    k(nullptr),
    cipher(nullptr)
  {}

  Authority::Authority(elle::io::Path const& path):
    k(nullptr),
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
    ELLE_TRACE_METHOD(pass);

    cryptography::SecretKey key{pass};

    ELLE_ASSERT(this->type == Authority::TypePair);

    delete this->cipher;
    this->cipher = nullptr;
    this->cipher = new cryptography::Cipher{key.encrypt(*this->k)};

    return elle::Status::Ok;
  }

  ///
  /// this method decrypts the keys.
  ///
  elle::Status          Authority::Decrypt(const elle::String&          pass)
  {
    ELLE_TRACE_METHOD(pass);

    ELLE_ASSERT(this->type == Authority::TypePair);
    ELLE_ASSERT(this->cipher != nullptr);

    cryptography::SecretKey key{pass};

    delete this->k;
    this->k = nullptr;
    this->k =
      new cryptography::PrivateKey{
        key.decrypt<cryptography::PrivateKey>(*this->cipher)};

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

    std::cout << alignment << elle::io::Dumpable::Shift
              << "[K] " << this->_K << std::endl;

    // if present...
    if (this->k != nullptr)
      {
        std::cout << alignment << elle::io::Dumpable::Shift
                  << "[k] " << *this->k << std::endl;
      }

    // dump the cipher.
    if (this->cipher != nullptr)
      {
        std::cout << alignment << elle::io::Dumpable::Shift
                  << "[Cipher] " << *this->cipher << std::endl;
      }

    return elle::Status::Ok;
  }
}
