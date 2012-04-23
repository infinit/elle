#include <elle/io/File.hh>

#include <lune/Authority.hh>
#include <lune/AuthoritySerializer.hxx>
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
    k(NULL),
    cipher(NULL)
  {
  }

  ///
  /// destructor.
  ///
  Authority::~Authority()
  {
    // release the private key, if present.
    if (this->k != NULL)
      delete this->k;

    // release the cipher.
    if (this->cipher != NULL)
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

    // allocate the cipher.
    this->cipher = new elle::cryptography::Cipher;

    // encrypt depending on the type.
    switch (this->type)
      {
      case Authority::TypePair:
        {
          // encrypt the authority.
          if (key.Encrypt(this->K, *this->k,
                          *this->cipher) == elle::Status::Error)
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
      default:
        {
          escape("unknown type");
        }
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
    if (this->cipher == NULL)
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
          if (key.Decrypt(*this->cipher,
                          this->K, *this->k) == elle::Status::Error)
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
      default:
        {
          escape("unknown type");
        }
      }

    return elle::Status::Ok;
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
    std::cout << alignment << elle::Dumpable::Shift
              << "[Type] " << this->type << std::endl;

    // dump the public key.
    if (this->K.Dump(margin + 2) == elle::Status::Error)
      escape("unable to dump the public key");

    // if present...
    if (this->k != NULL)
      {
        // ...dump the private key.
        if (this->k->Dump(margin + 2) == elle::Status::Error)
          escape("unable to dump the private key");
      }

    // dump the cipher.
    if (this->cipher != NULL)
      {
        if (this->cipher->Dump(margin + 2) == elle::Status::Error)
          escape("unable to dump the cipher");
      }

    return elle::Status::Ok;
  }

//
// ---------- archivable ------------------------------------------------------
//

  ///
  /// this method serializes the object.
  ///
  //elle::Status          Authority::Serialize(elle::Archive&     archive) const
  //{
  //  // check the cipher.
  //  if (this->cipher == NULL)
  //    escape("unable to serialize an unencrypted authority");

  //  // serialize the type and cipher.
  //  if (archive.Serialize(static_cast<elle::Natural8>(this->type),
  //                        *this->cipher) == elle::Status::Error)
  //    escape("unable to serialize the attributes");

  //  return elle::Status::Ok;
  //}

  /////
  ///// this method extracts the object.
  /////
  //elle::Status          Authority::Extract(elle::Archive&       archive)
  //{
  //  elle::Natural8      type;

  //  // allocate the cipher.
  //  this->cipher = new elle::cryptography::Cipher;

  //  // extract the type and cipher.
  //  if (archive.Extract(type, *this->cipher) == elle::Status::Error)
  //    escape("unable to extract the attributes");

  //  // set the type.
  //  this->type = static_cast<Authority::Type>(type);

  //  return elle::Status::Ok;
  //}

//
// ---------- fileable --------------------------------------------------------
//

  ///
  /// this method loads an authority file.
  ///
  //elle::Status          Authority::Load(elle::Path const& path)
  //{
  //  elle::standalone::Region        region;

  //  // read the file's content.
  //  if (elle::io::File::Read(path, region) == elle::Status::Error)
  //    escape("unable to read the file's content");

  //  // decode and extract the object.
  //  auto status = elle::Hexadecimal::Decode(
  //      // XXX this copy the whole buffer, not opti
  //      elle::String(reinterpret_cast<char*>(region.contents), region.size),
  //      *this
  //  );

  //  if (status == elle::Status::Error)
  //    escape("unable to decode the object");

  //  return elle::Status::Ok;
  //}

  ///
  /// this method loads the system's authority file.
  ///
  elle::Status          Authority::Load()
  {
    elle::Path          path;

    // create the path.
    if (path.Create(Lune::Authority) == elle::Status::Error)
      escape("unable to create the path");

    return this->Load(path);
  }

  ///
  /// this method stores the authority.
  ///
  elle::Status          Authority::Store() const
  {
    elle::Path          path;
    elle::standalone::Region        region;
    elle::String        string;

    // create the path.
    if (path.Create(Lune::Authority) == elle::Status::Error)
      escape("unable to create the path");

    if (this->Store(path) == elle::Status::Error)
      escape("unable to store");

    return elle::Status::Ok;
  }

  ///
  /// this method erases the authority.
  ///
  elle::Status          Authority::Erase() const
  {
    elle::Path          path;

    // create the path.
    if (path.Create(Lune::Authority) == elle::Status::Error)
      escape("unable to create the path");

    // erase the file.
    if (elle::io::File::Erase(path) == elle::Status::Error)
      escape("unable to erase the file");

    return elle::Status::Ok;
  }

  ///
  /// this method tests the authority.
  ///
  elle::Status          Authority::Exist() const
  {
    elle::Path          path;

    // create the path.
    if (path.Create(Lune::Authority) == elle::Status::Error)
      escape("unable to create the path");

    // test the file.
    if (elle::io::File::Exist(path) == elle::Status::False)
      return elle::Status::False;

    return elle::Status::True;
  }

}
