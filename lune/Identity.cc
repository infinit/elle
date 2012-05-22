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

#include <lune/Identity.hh>
#include <lune/Lune.hh>

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
    cipher(NULL)
  {
  }

  ///
  /// destructor.
  ///
  Identity::~Identity()
  {
    // release the cipher.
    if (this->cipher != NULL)
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
                                         const elle::KeyPair&   pair)
  {
    // One does not simply ...
    assert(pair.k.key() != nullptr);
    assert(pair.K.key() != nullptr);

    this->_id = id;
    this->name = name;
    this->pair = pair;

    assert(this->pair.k.key() != nullptr);
    assert(this->pair.K.key() != nullptr);

    return elle::StatusOk;
  }

  ///
  /// this method encrypts the key pair.
  ///
  elle::Status          Identity::Encrypt(const elle::String&   pass)
  {
    elle::SecretKey     key;

    // create a secret key with this pass.
    if (key.Create(pass) == elle::StatusError)
      escape("unable to create the secret key");

    // allocate the cipher.
    this->cipher = new elle::Cipher;

    // encrypt the authority.
    if (key.Encrypt(this->pair,
                    *this->cipher) == elle::StatusError)
      escape("unable to encrypt the key pair");

    return elle::StatusOk;
  }

  ///
  /// this method decrypts the key pair.
  ///
  elle::Status          Identity::Decrypt(const elle::String&   pass)
  {
    elle::SecretKey     key;

    // check the cipher.
    if (this->cipher == NULL)
      escape("unable to decrypt an unencrypted identity");

    // create a secret key with this pass.
    if (key.Create(pass) == elle::StatusError)
      escape("unable to create the secret key");

    // decrypt the authority.
    if (key.Decrypt(*this->cipher,
                    this->pair) == elle::StatusError)
      escape("unable to decrypt the key pair");

    return elle::StatusOk;
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

    return elle::StatusOk;
  }

  ///
  /// this method seals the identity with the authority.
  ///
  elle::Status          Identity::Seal(const Authority&         authority)
  {
    // check the cipher.
    if (this->cipher == NULL)
      escape("unable to seal an unencrypted identity");

    // sign the pair with the authority.
    if (authority.k->Sign(this->_id, this->name, *this->cipher,
                          this->signature) == elle::StatusError)
      escape("unable to sign the pair with the authority");

    return elle::StatusOk;
  }

  ///
  /// this method verifies the validity of the identity.
  ///
  elle::Status          Identity::Validate(const Authority&     authority)
    const
  {
    // check the cipher.
    if (this->cipher == NULL)
      escape("unable to verify an unencrypted identity");

    // verify the signature.
    if (authority.K.Verify(this->signature,
                           this->_id, this->name, *this->cipher) ==
        elle::StatusError)
      escape("unable to verify the signature");

    return elle::StatusOk;
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
    std::cout << alignment << elle::Dumpable::Shift
              << "[Id] " << this->_id << std::endl;

    // dump the name.
    std::cout << alignment << elle::Dumpable::Shift
              << "[Name] " << this->name << std::endl;

    // dump the pair.
    if (this->pair.Dump(margin + 2) == elle::StatusError)
      escape("unable to dump the pair");

    // dump the signature.
    if (this->signature.Dump(margin + 2) == elle::StatusError)
      escape("unable to dump the signature");

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
  elle::Status          Identity::Serialize(elle::Archive&      archive) const
  {
    // check the cipher.
    if (this->cipher != nullptr)
      {
        // serialize the attributes.
        if (archive.Serialize(
              static_cast<elle::Byte>(Identity::ModeEncrypted),
              this->_id,
              this->name,
              *this->cipher,
              this->signature) == elle::StatusError)
          escape("unable to serialize the attributes");
      }
    else
      {
        //
        // this is a special case where the identity is actually serialized
        // in its unencrypted form.
        //

        // serialize the attributes.
        if (archive.Serialize(
              static_cast<elle::Byte>(Identity::ModeUnencrypted),
              this->_id,
              this->name,
              this->pair,
              this->signature) == elle::StatusError)
          escape("unable to serialize the attributes");
      }

    return elle::StatusOk;
  }

  ///
  /// this method extracts the object.
  ///
  elle::Status          Identity::Extract(elle::Archive&        archive)
  {
    elle::Byte          mode;

    // allocate the cipher.
    this->cipher = new elle::Cipher;

    // extract the mode.
    if (archive.Extract(mode) == elle::StatusError)
      escape("unable to extract the attributes");

    // depending on the mode.
    switch (static_cast<Identity::Mode>(mode))
      {
      case Identity::ModeEncrypted:
        {
          // extract the attributes.
          if (archive.Extract(this->_id,
                              this->name,
                              *this->cipher,
                              this->signature) == elle::StatusError)
            escape("unable to extract the attributes");

          break;
        }
      case Identity::ModeUnencrypted:
        {
          // extract the attributes.
          if (archive.Extract(this->_id,
                              this->name,
                              this->pair,
                              this->signature) == elle::StatusError)
            escape("unable to extract the attributes");

          break;
        }
      }

    return elle::StatusOk;
  }

//
// ---------- fileable --------------------------------------------------------
//

  ///
  /// this method loads the current user's identity file.
  ///
  elle::Status          Identity::Load()
  {
    elle::Path          path;
    elle::Region        region;

    // create the path.
    if (path.Create(Lune::Identity) == elle::StatusError)
      escape("unable to create the path");

    // read the file's content.
    if (elle::File::Read(path, region) == elle::StatusError)
      escape("unable to read the file's content");

    // decode and extract the object.
    elle::String        data(reinterpret_cast<char const*>(region.contents),
                             region.size);

    if (elle::Hexadecimal::Decode(data, *this) == elle::StatusError)
      escape("unable to decode the object");

    return elle::StatusOk;
  }

  ///
  /// this method stores the current user's identity.
  ///
  elle::Status          Identity::Store() const
  {
    elle::Path          path;
    elle::Region        region;
    elle::String        string;

    // create the path.
    if (path.Create(Lune::Identity) == elle::StatusError)
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
  /// this method erases the current user's identity.
  ///
  elle::Status          Identity::Erase() const
  {
    elle::Path          path;

    // create the path.
    if (path.Create(Lune::Identity) == elle::StatusError)
      escape("unable to create the path");

    // erase the file.
    if (elle::File::Erase(path) == elle::StatusError)
      escape("unable to erase the file");

    return elle::StatusOk;
  }

  ///
  /// this method tests the current user's identity.
  ///
  elle::Status          Identity::Exist() const
  {
    elle::Path          path;

    // create the path.
    if (path.Create(Lune::Identity) == elle::StatusError)
      escape("unable to create the path");

    // test the file.
    if (elle::File::Exist(path) == elle::StatusFalse)
      return elle::StatusFalse;

    return elle::StatusTrue;
  }

  ///
  /// this method loads a user's identity file.
  ///
  elle::Status          Identity::Load(const elle::String&      name)
  {
    elle::Path          path;
    elle::Region        region;

    // create the path.
    if (path.Create(Lune::User::Identity) == elle::StatusError)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::Piece("%USER%", name)) == elle::StatusError)
      escape("unable to complete the path");

    // read the file's content.
    if (elle::File::Read(path, region) == elle::StatusError)
      escape("unable to read the file's content");

    // decode and extract the object.
    if (elle::Hexadecimal::Decode(
          elle::String(reinterpret_cast<char*>(region.contents),
                       region.size),
          *this) == elle::StatusError)
      escape("unable to decode the object");

    return elle::StatusOk;
  }

  ///
  /// this method stores a user's identity.
  ///
  elle::Status          Identity::Store(const elle::String&     name) const
  {
    elle::Path          path;
    elle::Region        region;
    elle::String        string;

    // create the path.
    if (path.Create(Lune::User::Identity) == elle::StatusError)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::Piece("%USER%", name)) == elle::StatusError)
      escape("unable to complete the path");

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
  /// this method erases a user's identity.
  ///
  elle::Status          Identity::Erase(const elle::String&     name) const
  {
    elle::Path          path;

    // create the path.
    if (path.Create(Lune::User::Identity) == elle::StatusError)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::Piece("%USER%", name)) == elle::StatusError)
      escape("unable to complete the path");

    // erase the file.
    if (elle::File::Erase(path) == elle::StatusError)
      escape("unable to erase the file");

    return elle::StatusOk;
  }

  ///
  /// this method tests the identity.
  ///
  elle::Status          Identity::Exist(const elle::String&     name) const
  {
    elle::Path          path;

    // create the path.
    if (path.Create(Lune::User::Identity) == elle::StatusError)
      escape("unable to create the path");

    // complete the path's pattern.
    if (path.Complete(elle::Piece("%USER%", name)) == elle::StatusError)
      escape("unable to complete the path");

    // test the file.
    if (elle::File::Exist(path) == elle::StatusFalse)
      return elle::StatusFalse;

    return elle::StatusTrue;
  }

}
