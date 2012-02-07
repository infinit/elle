//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [tue oct 30 10:07:31 2007]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/cryptography/PrivateKey.hh>
#include <elle/cryptography/OneWay.hh>
#include <elle/cryptography/SecretKey.hh>

#include <comet/Comet.hh>

using namespace elle;
using namespace elle::cryptography;

//
// ---------- definitions -----------------------------------------------------
//

///
/// this defines a null private key.
///
const PrivateKey            PrivateKey::Null;

//
// ---------- constructors & destructors --------------------------------------
//

///
/// this method initializes the object.
///
PrivateKey::PrivateKey():
  _key(nullptr), _contexts{nullptr, nullptr, nullptr}
{}

///
/// this is the copy constructor.
///
PrivateKey::PrivateKey(PrivateKey const&k) :
  Object(k)
{
  assert(k._key != nullptr);

  // create the private key by duplicating the internal numbers.
  if (this->Create(k._key) == StatusError)
    fail("unable to duplicate the private key");

  assert(this->_key != nullptr);
}

///
/// this method releases the resources.
///
PrivateKey::~PrivateKey()
{
  // release the resources.
  if (this->_key != nullptr)
    ::EVP_PKEY_free(this->_key);

  if (this->_contexts.decrypt != nullptr)
    ::EVP_PKEY_CTX_free(this->_contexts.decrypt);

  if (this->_contexts.sign != nullptr)
    ::EVP_PKEY_CTX_free(this->_contexts.sign);

  if (this->_contexts.encrypt != nullptr)
    ::EVP_PKEY_CTX_free(this->_contexts.encrypt);
}

//
// ---------- methods ---------------------------------------------------------
//

///
/// this method constructs a valid private key.
///
Status              PrivateKey::Create(const ::EVP_PKEY*    key)
{
  assert(this->_key == nullptr);

  if (this->Create(::BN_dup(key->pkey.rsa->n),
                   ::BN_dup(key->pkey.rsa->e),
                   ::BN_dup(key->pkey.rsa->d),
                   ::BN_dup(key->pkey.rsa->p),
                   ::BN_dup(key->pkey.rsa->q),
                   ::BN_dup(key->pkey.rsa->dmp1),
                   ::BN_dup(key->pkey.rsa->dmq1),
                   ::BN_dup(key->pkey.rsa->iqmp)) == StatusError)
    escape("unable to create the private key");

  assert(this->_key != nullptr);

  return elle::StatusOk;
}

///
/// this method constructs a valid private key given the proper
/// numbers.
///
Status              PrivateKey::Create(Large*               n,
                                       Large*               e,
                                       Large*               d,
                                       Large*               p,
                                       Large*               q,
                                       Large*               dmp1,
                                       Large*               dmq1,
                                       Large*               iqmp)
{
  ::RSA*            rsa;

  //
  // key
  //

  assert(this->_key == nullptr);

  // initialise the private key structure.
  if ((this->_key = ::EVP_PKEY_new()) == nullptr)
    escape(::ERR_error_string(ERR_get_error(), nullptr));

  // create the RSA structure.
  if ((rsa = ::RSA_new()) == nullptr)
      escape(::ERR_error_string(ERR_get_error(), nullptr));


  // assign the big numbers relevant to the private key.
  rsa->n = n;
  rsa->e = e;
  rsa->d = d;
  rsa->p = p;
  rsa->q = q;
  rsa->dmp1 = dmp1;
  rsa->dmq1 = dmq1;
  rsa->iqmp = iqmp;

  // set the rsa structure into the private key.
  if (::EVP_PKEY_assign_RSA(this->_key, rsa) <= 0)
    {
      ::RSA_free(rsa);
      escape(::ERR_error_string(ERR_get_error(), nullptr));
    }

  //
  // contexts
  //

  // create, initialize and configure the decrypt context.
  if ((this->_contexts.decrypt = ::EVP_PKEY_CTX_new(this->_key, nullptr)) == nullptr)
    escape(::ERR_error_string(ERR_get_error(), nullptr));

  if (::EVP_PKEY_decrypt_init(this->_contexts.decrypt) <= 0)
    escape(::ERR_error_string(ERR_get_error(), nullptr));

  if (::EVP_PKEY_CTX_ctrl(this->_contexts.decrypt,
                          EVP_PKEY_RSA,
                          -1,
                          EVP_PKEY_CTRL_RSA_PADDING,
                          RSA_PKCS1_OAEP_PADDING,
                          nullptr) <= 0)
    escape(::ERR_error_string(ERR_get_error(), nullptr));

  // create and initialize a sign context.
  if ((this->_contexts.sign =
       ::EVP_PKEY_CTX_new(this->_key, nullptr)) == nullptr)
    escape(::ERR_error_string(ERR_get_error(), nullptr));

  if (::EVP_PKEY_sign_init(this->_contexts.sign) <= 0)
    escape(::ERR_error_string(ERR_get_error(), nullptr));

  if (::EVP_PKEY_CTX_ctrl(this->_contexts.sign,
                          EVP_PKEY_RSA,
                          -1,
                          EVP_PKEY_CTRL_RSA_PADDING,
                          RSA_PKCS1_PADDING,
                          nullptr) <= 0)
    escape(::ERR_error_string(ERR_get_error(), nullptr));

  // create and initialize a encrypt context.
  if ((this->_contexts.encrypt =
       ::EVP_PKEY_CTX_new(this->_key, nullptr)) == nullptr)
    escape(::ERR_error_string(ERR_get_error(), nullptr));

  if (::EVP_PKEY_sign_init(this->_contexts.encrypt) <= 0)
    escape(::ERR_error_string(ERR_get_error(), nullptr));

  if (::EVP_PKEY_CTX_ctrl(this->_contexts.encrypt,
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
/// this method decrypts a code which should actually be
/// an archive containing both a secret key and some data.
///
/// this method starts by (i) extracting the key and data
/// in their encrypted forms (ii) decrypt the symmetric key
/// with the private key and (iii) decipher the data with the
/// symmetric key.
///
Status              PrivateKey::Decrypt(const Code&         code,
                                        Clear&              clear) const
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
    if (::EVP_PKEY_decrypt(
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
    if (::EVP_PKEY_decrypt(
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

///
/// this method signs a plain text.
///
Status              PrivateKey::Sign(const Plain&           plain,
                                     Signature&             signature)
  const
{
  Digest            digest;
  size_t            size;

  // compute the plain's digest.
  if (OneWay::Hash(plain, digest) == StatusError)
    escape("unable to hash the plain");

  // sign the portion.
  if (::EVP_PKEY_sign(
        this->_contexts.sign,
        nullptr,
        &size,
        reinterpret_cast<const unsigned char*>(digest.region.contents),
        digest.region.size) <= 0)
    escape(::ERR_error_string(ERR_get_error(), nullptr));

  // prepare the signature so it can receive the upcoming portion.
  if (signature.region.Prepare(size) == StatusError)
    escape("unable to prepare the signature");

  // actually sign the portion.
  if (::EVP_PKEY_sign(
        this->_contexts.sign,
        reinterpret_cast<unsigned char*>(signature.region.contents),
        &size,
        reinterpret_cast<const unsigned char*>(digest.region.contents),
        digest.region.size) <= 0)
    escape(::ERR_error_string(ERR_get_error(), nullptr));

  // set the code size.
  signature.region.size = size;

  return elle::StatusOk;
}

///
/// this method encrypts the given data with the private key.
///
/// although unusual, the private key can very well be used for
/// encrypting in which case the public key would be used for
/// decrypting.
///
/// since (i) the private key size limits the size of the data that
/// can be encrypted and (ii) raising large data to large exponent
/// is very slow; the algorithm below consists in (i) generating
/// a secret key, (ii) ciphering the plain text with this key,
/// (iii) encrypting the secret key with the private key and finally
/// (iv) returning an archive containing the asymetrically-encrypted
/// secret key with the symmetrically-encrypted data.
///
Status              PrivateKey::Encrypt(const Plain&        plain,
                                        Code&               code) const
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
    if (::EVP_PKEY_sign(
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
    //
    // note that since the encryption is performed with the private key,
    // the operation is equivalent to a signature.
    if (::EVP_PKEY_sign(
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
/// this method derives a public key according to (i) its complementary
/// private key and (ii) the seed used for rotating this key pair.
///
Status              PrivateKey::Derive(const Seed&          seed,
                                       PublicKey&           K) const
{
  struct Scope
    {
      ::EVP_PKEY*       key;
      ::RSA*            rsa;

      Scope() : key(nullptr), rsa(nullptr) {}
      ~Scope() { ::EVP_PKEY_free(this->key); ::RSA_free(this->rsa); }
    } scope;

  // create an EVP key.
  if ((scope.key = ::EVP_PKEY_new()) == nullptr)
    escape(::ERR_error_string(ERR_get_error(), nullptr));

  // create a new RSA key.
  if ((scope.rsa = ::RSA_new()) == nullptr)
    escape(::ERR_error_string(ERR_get_error(), nullptr));

  // derive the RSA key.
  if (comet::RSA_derive(scope.rsa,
                        this->_key->pkey.rsa->n,
                        seed.region.contents,
                        seed.region.size) <= 0)
    escape(::ERR_error_string(ERR_get_error(), nullptr));

  // assign the RSA key to the EVP's.
  if (::EVP_PKEY_assign_RSA(scope.key, scope.rsa) <= 0)
    escape(::ERR_error_string(ERR_get_error(), nullptr));

  // stop tracking.
  scope.rsa = nullptr;

  // create the rotated public key according to the EVP structure.
  if (K.Create(scope.key) == StatusError)
    escape("unable to create the public key");

  return elle::StatusOk;
}

//
// ---------- object ----------------------------------------------------------
//

///
/// this method check if two objects match.
///
Boolean             PrivateKey::operator==(const PrivateKey& element) const
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
      if ((::BN_cmp(this->_key->pkey.rsa->n,
                    element._key->pkey.rsa->n) != 0) ||
          (::BN_cmp(this->_key->pkey.rsa->d,
                    element._key->pkey.rsa->d) != 0) ||
          (::BN_cmp(this->_key->pkey.rsa->p,
                    element._key->pkey.rsa->p) != 0) ||
          (::BN_cmp(this->_key->pkey.rsa->q,
                    element._key->pkey.rsa->q) != 0))
        return elle::StatusFalse;
    }

  return elle::StatusTrue;
}

///
/// this macro-function call generates the object.
///
embed(PrivateKey, _());

//
// ---------- dumpable --------------------------------------------------------
//

///
/// this method dumps the private key internals.
///
Status              PrivateKey::Dump(const Natural32        margin) const
{
  String            alignment(margin, ' ');

  // display depending on the value.
  if (*this == PrivateKey::Null)
    {
      std::cout << alignment << "[PrivateKey] " << none << std::endl;
    }
  else
    {
      std::cout << alignment << "[PrivateKey]" << std::endl;

      // dump the internal numbers.
      std::cout << alignment << Dumpable::Shift << "[n] "
                << *this->_key->pkey.rsa->n << std::endl;

      std::cout << alignment << Dumpable::Shift << "[d] "
                << *this->_key->pkey.rsa->d << std::endl;

      std::cout << alignment << Dumpable::Shift << "[p] "
                << *this->_key->pkey.rsa->p << std::endl;

      std::cout << alignment << Dumpable::Shift << "[q] "
                << *this->_key->pkey.rsa->q << std::endl;
    }

  return elle::StatusOk;
}

//
// ---------- archivable ------------------------------------------------------
//

///
/// this method serializes a private key object.
///
Status              PrivateKey::Serialize(Archive&          archive) const
{
  // serialize the internal numbers.
  if (archive.Serialize(*this->_key->pkey.rsa->n,
                        *this->_key->pkey.rsa->e,
                        *this->_key->pkey.rsa->d,
                        *this->_key->pkey.rsa->p,
                        *this->_key->pkey.rsa->q,
                        *this->_key->pkey.rsa->dmp1,
                        *this->_key->pkey.rsa->dmq1,
                        *this->_key->pkey.rsa->iqmp) == StatusError)
    escape("unable to serialize the internal numbers");

  return elle::StatusOk;
}

///
/// this method extract a private key from the given archive.
///
Status              PrivateKey::Extract(Archive&            archive)
{
  struct OnExit
    {
      bool              track;
      Large             n;
      Large             e;
      Large             d;
      Large             p;
      Large             q;
      Large             dmp1;
      Large             dmq1;
      Large             iqmp;
      OnExit() : track(false) {}
      ~OnExit()
      {
        if (!this->track)
          return;
        ::BN_clear_free(&this->n);
        ::BN_clear_free(&this->e);
        ::BN_clear_free(&this->d);
        ::BN_clear_free(&this->p);
        ::BN_clear_free(&this->q);
        ::BN_clear_free(&this->dmp1);
        ::BN_clear_free(&this->dmq1);
        ::BN_clear_free(&this->iqmp);
      }
    } scope;


  // extract the numbers.
  if (archive.Extract(scope.n, scope.e, scope.d, scope.p, scope.q,
                      scope.dmp1, scope.dmq1, scope.iqmp) == StatusError)
    escape("unable to extract the internal numbers");

  // track the local variables' deallocation.
  scope.track = true;

  // create the EVP_PKEY object from the extract numbers.
  if (this->Create(::BN_dup(&scope.n),
                   ::BN_dup(&scope.e),
                   ::BN_dup(&scope.d),
                   ::BN_dup(&scope.p),
                   ::BN_dup(&scope.q),
                   ::BN_dup(&scope.dmp1),
                   ::BN_dup(&scope.dmq1),
                   ::BN_dup(&scope.iqmp)) == StatusError)
    escape("unable to create the private key from the archive");

  assert(this->_key != nullptr);

  return elle::StatusOk;
}

