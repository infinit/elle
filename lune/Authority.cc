//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// author        julien quintard   [tue may  4 23:47:55 2010]
//

//
// ---------- includes --------------------------------------------------------
//

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
    this->k = new elle::PrivateKey(pair.k);

    return elle::StatusOk;
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

    return elle::StatusOk;
  }

  ///
  /// this method encrypts the keys.
  ///
  elle::Status          Authority::Encrypt(const elle::String&          pass)
  {
    elle::SecretKey     key;

    // create a secret key with this pass.
    if (key.Create(pass) == elle::StatusError)
      escape("unable to create the secret key");

    // allocate the cipher.
    this->cipher = new elle::Cipher;

    // encrypt depending on the type.
    switch (this->type)
      {
      case Authority::TypePair:
        {
          // encrypt the authority.
          if (key.Encrypt(this->K, *this->k,
                          *this->cipher) == elle::StatusError)
            escape("unable to encrypt the authority");

          break;
        }
      case Authority::TypePublic:
        {
          // encrypt the authority.
          if (key.Encrypt(this->K,
                          *this->cipher) == elle::StatusError)
            escape("unable to encrypt the authority");

          break;
        }
      case Authority::TypeUnknown:
      default:
        {
          escape("unknown type");
        }
      }

    return elle::StatusOk;
  }

  ///
  /// this method decrypts the keys.
  ///
  elle::Status          Authority::Decrypt(const elle::String&          pass)
  {
    elle::SecretKey     key;

    // check the cipher.
    if (this->cipher == NULL)
      escape("unable to decrypt an unencrypted authority");

    // create a secret key with this pass.
    if (key.Create(pass) == elle::StatusError)
      escape("unable to create the secret key");

    // decrypt depending on the type.
    switch (this->type)
      {
      case Authority::TypePair:
        {
          // allocate the private key.
          this->k = new elle::PrivateKey;

          // decrypt the authority.
          if (key.Decrypt(*this->cipher,
                          this->K, *this->k) == elle::StatusError)
            escape("unable to decrypt the authority");

          break;
        }
      case Authority::TypePublic:
        {
          // decrypt the authority.
          if (key.Decrypt(*this->cipher,
                          this->K) == elle::StatusError)
            escape("unable to decrypt the authority");

          break;
        }
      case Authority::TypeUnknown:
      default:
        {
          escape("unknown type");
        }
      }

    return elle::StatusOk;
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
    if (this->K.Dump(margin + 2) == elle::StatusError)
      escape("unable to dump the public key");

    // if present...
    if (this->k != NULL)
      {
        // ...dump the private key.
        if (this->k->Dump(margin + 2) == elle::StatusError)
          escape("unable to dump the private key");
      }

    // dump the cipher.
    if (this->cipher != NULL)
      {
        if (this->cipher->Dump(margin + 2) == elle::StatusError)
          escape("unable to dump the cipher");
      }

    return elle::StatusOk;
  }

//
// ---------- archivable ------------------------------------------------------
//

  ///
  /// this method serializes the object.
  ///
  elle::Status          Authority::Serialize(elle::Archive&     archive) const
  {
    // check the cipher.
    if (this->cipher == NULL)
      escape("unable to serialize an unencrypted authority");

    // serialize the type and cipher.
    if (archive.Serialize(static_cast<elle::Natural8>(this->type),
                          *this->cipher) == elle::StatusError)
      escape("unable to serialize the attributes");

    return elle::StatusOk;
  }

  ///
  /// this method extracts the object.
  ///
  elle::Status          Authority::Extract(elle::Archive&       archive)
  {
    elle::Natural8      type;

    // allocate the cipher.
    this->cipher = new elle::Cipher;

    // extract the type and cipher.
    if (archive.Extract(type, *this->cipher) == elle::StatusError)
      escape("unable to extract the attributes");

    // set the type.
    this->type = static_cast<Authority::Type>(type);

    return elle::StatusOk;
  }

//
// ---------- fileable --------------------------------------------------------
//

  ///
  /// this method loads an authority file.
  ///
  elle::Status          Authority::Load(elle::Path const& path)
  {
    elle::Region        region;

    // read the file's content.
    if (elle::File::Read(path, region) == elle::StatusError)
      escape("unable to read the file's content");

    // decode and extract the object.
    auto status = elle::Hexadecimal::Decode(
        // XXX this copy the whole buffer, not opti
        elle::String(reinterpret_cast<char*>(region.contents), region.size),
        *this
    );

    if (status == elle::StatusError)
      escape("unable to decode the object");

    return elle::StatusOk;
  }

  ///
  /// this method loads the system's authority file.
  ///
  elle::Status          Authority::Load()
  {
    elle::Path          path;

    // create the path.
    if (path.Create(Lune::Authority) == elle::StatusError)
      escape("unable to create the path");

    return this->Load(path);
  }

  ///
  /// this method stores the authority.
  ///
  elle::Status          Authority::Store() const
  {
    elle::Path          path;
    elle::Region        region;
    elle::String        string;

    // create the path.
    if (path.Create(Lune::Authority) == elle::StatusError)
      escape("unable to create the path");

    // encode in hexadecimal.
    if (elle::Hexadecimal::Encode(*this, string) == elle::StatusError)
      escape("unable to encode the object in hexadecimal");

    // wrap the string.
    if (region.Wrap(reinterpret_cast<const elle::Byte*>(string.c_str()),
                    string.length()) == elle::StatusError)
      escape("unable to wrap the string in a region");

    // write the file's content.
    if (elle::File::Write(path, region) == elle::StatusError)
      escape("unable to write the file's content");

    return elle::StatusOk;
  }

  ///
  /// this method erases the authority.
  ///
  elle::Status          Authority::Erase() const
  {
    elle::Path          path;

    // create the path.
    if (path.Create(Lune::Authority) == elle::StatusError)
      escape("unable to create the path");

    // erase the file.
    if (elle::File::Erase(path) == elle::StatusError)
      escape("unable to erase the file");

    return elle::StatusOk;
  }

  ///
  /// this method tests the authority.
  ///
  elle::Status          Authority::Exist() const
  {
    elle::Path          path;

    // create the path.
    if (path.Create(Lune::Authority) == elle::StatusError)
      escape("unable to create the path");

    // test the file.
    if (elle::File::Exist(path) == elle::StatusFalse)
      return elle::StatusFalse;

    return elle::StatusTrue;
  }

}
