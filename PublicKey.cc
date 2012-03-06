//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [tue oct 30 01:23:20 2007]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/cryptography/PublicKey.hh>
#include <elle/cryptography/Digest.hh>
#include <elle/cryptography/OneWay.hh>
#include <elle/cryptography/SecretKey.hh>

using namespace elle;
using namespace elle::cryptography;

//
// ---------- definitions -----------------------------------------------------
//

///
/// this defines a null public key.
///
const PublicKey             PublicKey::Null;

//
// ---------- constructors & destructors --------------------------------------
//

///
/// this method initializes the object.
///
PublicKey::PublicKey():
  _key(nullptr)
{
  // initialize the contexts.
  this->_contexts.encrypt = nullptr;
  this->_contexts.verify = nullptr;
  this->_contexts.decrypt = nullptr;
}

///
/// this is the copy constructor.
///
PublicKey::PublicKey(const PublicKey& K) :
  Object(K), _key(nullptr)
{
  // re-create the public key by duplicate the internal numbers.
  if (this->Create(K._key) == StatusError)
    fail("unable to duplicate the public key");
}

///
/// this method releases the resources.
///
PublicKey::~PublicKey()
{
  // release the resources.
  if (this->_key != nullptr)
    ::EVP_PKEY_free(this->_key);

  if (this->_contexts.encrypt != nullptr)
    ::EVP_PKEY_CTX_free(this->_contexts.encrypt);

  if (this->_contexts.verify != nullptr)
    ::EVP_PKEY_CTX_free(this->_contexts.verify);

  if (this->_contexts.decrypt != nullptr)
    ::EVP_PKEY_CTX_free(this->_contexts.decrypt);
}

//
// ---------- methods ---------------------------------------------------------
//

#include "backtrace.cc"

///
/// this method constructs a valid public key.
///
Status PublicKey::Create(::EVP_PKEY const* key)
{
  if (key == nullptr)
    std::cerr << nitro::Backtrace() << " >BITE\n";
  assert(key != nullptr);
  assert(this->_key == nullptr);

  // call the creation method.
  if (this->Create(::BN_dup(key->pkey.rsa->n),
                   ::BN_dup(key->pkey.rsa->e)) == StatusError)
    escape("unable to create the public key");

  return elle::StatusOk;
}

///
/// this method constructs a valid public key given the proper
/// numbers.
///
Status PublicKey::Create(Large* n, Large* e)
{
  assert(n != nullptr);
  assert(e != nullptr);

  // initialise the public key structure.
  if ((this->_key = ::EVP_PKEY_new()) == nullptr)
    escape(::ERR_error_string(ERR_get_error(), nullptr));

  ::RSA* rsa;
  // create the RSA structure.
  if ((rsa = ::RSA_new()) == nullptr)
    {
      ::RSA_free(rsa);
      escape(::ERR_error_string(ERR_get_error(), nullptr));
    }

  // assign the big numbers relevant to the public key.
  rsa->n = n;
  rsa->e = e;

  // set the rsa structure into the public key.
  if (::EVP_PKEY_assign_RSA(this->_key, rsa) <= 0)
    {
      ::RSA_free(rsa);
      escape(::ERR_error_string(ERR_get_error(), nullptr));
    }

  //
  // contexts
  //

  // create, initialize and configure---by setting the padding---the
  // encrypt context.
  this->_contexts.encrypt = ::EVP_PKEY_CTX_new(this->_key, nullptr);
  if (this->_contexts.encrypt == nullptr)
    escape(::ERR_error_string(ERR_get_error(), nullptr));

  if (::EVP_PKEY_encrypt_init(this->_contexts.encrypt) <= 0)
    escape(::ERR_error_string(ERR_get_error(), nullptr));

  if (::EVP_PKEY_CTX_ctrl(this->_contexts.encrypt,
                          EVP_PKEY_RSA,
                          -1,
                          EVP_PKEY_CTRL_RSA_PADDING,
                          RSA_PKCS1_OAEP_PADDING,
                          nullptr) <= 0)
    escape(::ERR_error_string(ERR_get_error(), nullptr));

  // create and initialize the verify context.
  this->_contexts.verify = EVP_PKEY_CTX_new(this->_key, nullptr);
  if (this->_contexts.verify == nullptr)
    escape(::ERR_error_string(ERR_get_error(), nullptr));

  if (::EVP_PKEY_verify_init(this->_contexts.verify) <= 0)
    escape(::ERR_error_string(ERR_get_error(), nullptr));

  if (::EVP_PKEY_CTX_ctrl(this->_contexts.verify,
                          EVP_PKEY_RSA,
                          -1,
                          EVP_PKEY_CTRL_RSA_PADDING,
                          RSA_PKCS1_PADDING,
                          nullptr) <= 0)
    escape(::ERR_error_string(ERR_get_error(), nullptr));

  // create and initialize the decrypt context.
  this->_contexts.decrypt = ::EVP_PKEY_CTX_new(this->_key, nullptr);
  if (this->_contexts.decrypt == nullptr)
    escape(::ERR_error_string(ERR_get_error(), nullptr));

  if (::EVP_PKEY_verify_recover_init(this->_contexts.decrypt) <= 0)
    escape(::ERR_error_string(ERR_get_error(), nullptr));

  if (::EVP_PKEY_CTX_ctrl(this->_contexts.decrypt,
                          EVP_PKEY_RSA,
                          -1,
                          EVP_PKEY_CTRL_RSA_PADDING,
                          RSA_PKCS1_PADDING,
                          nullptr) <= 0)
    escape(::ERR_error_string(ERR_get_error(), nullptr));

  assert(this->_key != nullptr);

  return elle::StatusOk;
}

///
/// this method encrypts the given data with the public key.
///
/// since the public key size limits the size of the data that
/// can be encrypted and raising large data to large exponent
/// is very slow; the algorithm below consists in (i) generating
/// a secret key, (ii) ciphering the plain text with this key,
/// (iii) encrypting the secret key with the public key and finally
/// (iv) returning an archive containing the asymetrically-encrypted
/// secret key with the symmetrically-encrypted data.
///
/// note however that should the plain be small enough, (1) a direct
/// computation is performed. otherwise, (2) the data is symmetrically
/// encrypted.
///
Status              PublicKey::Encrypt(const Plain&         plain,
                                       Code&                code) const
{
  SecretKey         secret;

  Code              key;
  Cipher            data;

  // (i)
  {
    // generate a secret key.
    if (secret.Generate() == StatusError)
      escape("unable to generate the secret key");
  }

  // (ii)
  {
    // cipher the plain text with the secret key.
    if (secret.Encrypt(plain, data) == StatusError)
      escape("unable to cipher the plain text with the secret key");
  }

  // (iii)
  {
    Archive         archive;
    size_t          size;

    // first, create an archive.
    if (archive.Create() == StatusError)
      escape("unable to create an achive");

    // then, serialize the secret key.
    if (archive.Serialize(secret) == StatusError)
      escape("unable to serialize the secret key");

    // compute the size of the archived symmetric key.
    if (::EVP_PKEY_encrypt(
          this->_contexts.encrypt,
          nullptr,
          &size,
          reinterpret_cast<const unsigned char*>(archive.contents),
          archive.size) <= 0)
      escape(::ERR_error_string(ERR_get_error(), nullptr));

    // allocate memory so the key can receive the upcoming
    // encrypted portion.
    if (key.region.Prepare(size) == StatusError)
      escape("unable to prepare the key");

    // actually encrypt the secret key's archive, storing the encrypted
    // portion directly into the key object, without any re-copy.
    if (::EVP_PKEY_encrypt(
          this->_contexts.encrypt,
          reinterpret_cast<unsigned char*>(key.region.contents),
          &size,
          reinterpret_cast<const unsigned char*>(archive.contents),
          archive.size) <= 0)
      escape(::ERR_error_string(ERR_get_error(), nullptr));

    // set the key size.
    key.region.size = size;
  }

  // (iv)
  {
    Archive         archive;

    // create the main archive.
    if (archive.Create() == StatusError)
      escape("unable to create the archive");

    // serialize the key.
    if (archive.Serialize(key, data) == StatusError)
      escape("unable to serialize the asymetrically-encrypted secret key "
             "and the symetrically-encrypted data");

    // duplicate the archive's content.
    if (code.region.Duplicate(archive.contents,
                              archive.size) == StatusError)
      escape("unable to duplicate the archive's content");
  }

  return elle::StatusOk;
}

///
/// this method verifies that the non-signed text is equal to the
/// plain text.
///
/// note that, as for the Sign() method, this method computes
/// the plain's digest before forwarding to the other Verify()
/// method.
///
Status              PublicKey::Verify(const Signature&      signature,
                                      const Plain&          plain) const
{
  Digest            digest;

  // compute the plain's digest.
  if (OneWay::Hash(plain, digest) == StatusError)
    escape("unable to hash the plain");

  // verify.
  if (::EVP_PKEY_verify(
        this->_contexts.verify,
        reinterpret_cast<const unsigned char*>(signature.region.contents),
        signature.region.size,
        reinterpret_cast<const unsigned char*>(digest.region.contents),
        digest.region.size) <= 0)
    escape(::ERR_error_string(ERR_get_error(), nullptr));

  return elle::StatusOk;
}

///
/// this method decrypts a code which should actually be
/// an archive containing both a secret key and some data.
///
/// note that, although it may sound strange to 'decrypt' with a
/// public key, it does not matter as both keys act as the other's
/// opposite.
///
/// therefore the public key can be used to encrypt in which case
/// the private key will be used for decrypting or the other way
/// around, which is what this function is for.
///
/// this method starts by (i) extracting the key and data
/// in their encrypted forms (ii) decrypt the symmetric key
/// with the public key and (iii) decipher the data with the
/// symmetric key.
///
Status              PublicKey::Decrypt(const Code&          code,
                                       Clear&               clear) const
{
  SecretKey         secret;

  Archive           archive;
  Code              key;
  Cipher            data;

  // (i)
  {
    // prepare the archive.
    if (archive.Wrap(code.region) == StatusError)
      escape("unable to prepare the archive");

    // extract the secret key and data, in their encrypted form.
    if (archive.Extract(key, data) == StatusError)
      escape("unable to extract the asymetrically-encrypted secret key "
             "and the symetrically-encrypted data");
  }

  // (ii)
  {
    Archive         archive;
    Region          region;
    size_t          size;

    // compute the size of the decrypted portion to come.
    if (::EVP_PKEY_verify_recover(
          this->_contexts.decrypt,
          nullptr,
          &size,
          reinterpret_cast<const unsigned char*>(key.region.contents),
          key.region.size) <= 0)
      escape(::ERR_error_string(ERR_get_error(), nullptr));

    // allocate the required memory for the region object.
    if (region.Prepare(size) == StatusError)
      escape("unable to allocate the required memory");

    // perform the decrypt operation.
    //
    // note that since the encryption with the private key relies
    // upon the sign() EVP functionality, the verify_recover()
    // function is used here.
    if (::EVP_PKEY_verify_recover(
          this->_contexts.decrypt,
          reinterpret_cast<unsigned char*>(region.contents),
          &size,
          reinterpret_cast<const unsigned char*>(key.region.contents),
          key.region.size) <= 0)
      escape(::ERR_error_string(ERR_get_error(), nullptr));

    // set the region size.
    region.size = size;

    // prepare the archive.
    if (archive.Acquire(region) == StatusError)
      escape("unable to prepare the archive");

    // detach the data from the region so that the data
    // is not release twice by both 'region' and 'archive'.
    if (region.Detach() == StatusError)
      escape("unable to detach the data from the region");

    // extract the secret key.
    if (archive.Extract(secret) == StatusError)
      escape("unable to extract the secret key from the archive");
  }

  // (iii)
  {
    // finally, decrypt the data with the secret key.
    if (secret.Decrypt(data, clear) == StatusError)
      escape("unable to decrypt the data with the secret key");
  }

  return elle::StatusOk;
}

//
// ---------- object ----------------------------------------------------------
//

///
/// this method checks if two objects match.
///
Boolean PublicKey::operator==(const PublicKey& element) const
{
  // check the address as this may actually be the same object.
  if (this == &element)
    return elle::StatusTrue;

  // if one of the key is null....
  if ((this->_key == nullptr) || (element._key == nullptr))
    {
      // compare the addresses.
      if (this->_key != element._key)
        return elle::StatusFalse;
    }
  else
    {
      // compare the internal numbers.
      if ((::BN_cmp(this->_key->pkey.rsa->n, element._key->pkey.rsa->n) != 0) ||
          (::BN_cmp(this->_key->pkey.rsa->e, element._key->pkey.rsa->e) != 0))
        return elle::StatusFalse;
    }

  return elle::StatusTrue;
}

///
/// this macro-function call generates the object.
///
embed(PublicKey, _());

//
// ---------- dumpable --------------------------------------------------------
//

///
/// this method dumps the public key internals.
///
Status              PublicKey::Dump(const Natural32         margin) const
{
  String            alignment(margin, ' ');

  // display depending on the value.
  if (*this == PublicKey::Null)
    {
      std::cout << alignment << "[PublicKey] " << none << std::endl;
    }
  else
    {
      std::cout << alignment << "[PublicKey] " << std::endl;

      // dump the internal numbers.
      std::cout << alignment << Dumpable::Shift << "[n] "
                << *this->_key->pkey.rsa->n << std::endl;

      std::cout << alignment << Dumpable::Shift << "[e] "
                << *this->_key->pkey.rsa->e << std::endl;
    }

  return elle::StatusOk;
}

//
// ---------- archivable ------------------------------------------------------
//

///
/// this method serializes a public key object.
///
Status          PublicKey::Serialize(Archive&           archive) const
{
  // XXX assert(this->_key != nullptr);
  // XXX[to remove]
  if (this->_key == nullptr)
    escape("XXX");

  // serialize the internal numbers.
  if (archive.Serialize(*this->_key->pkey.rsa->n,
                        *this->_key->pkey.rsa->e) == StatusError)
    escape("unable to serialize the internal numbers");

  return elle::StatusOk;
}

///
/// this method extract a public key from the given archive.
///
Status          PublicKey::Extract(Archive&             archive)
{
  struct Scope
  {
    bool              track;
    Large             n;
    Large             e;
    Scope() : track(false) {}
    ~Scope()
    {
      if (this->track)
        {
          ::BN_clear_free(&this->n);
          ::BN_clear_free(&this->e);
        }
    }
  } scope;


  // extract the numbers.
  if (archive.Extract(scope.n, scope.e) == StatusError)
    escape("unable to extract the internal numbers");

  scope.track = true;

  // create the EVP_PKEY object from the extract numbers.
  if (this->Create(::BN_dup(&scope.n),
                   ::BN_dup(&scope.e)) == StatusError)
    escape("unable to create the public key from the archive");

  return elle::StatusOk;
}

