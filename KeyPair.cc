#include <elle/cryptography/KeyPair.hh>
#include <elle/cryptography/KeyPair.hh>
#include <elle/cryptography/Cipher.hh>
#include <elle/cryptography/SecretKey.hh>
#include <elle/cryptography/Seed.hh>
#include <elle/cryptography/cryptography.hh>

#include <elle/types.hh>
#include <elle/Exception.hh>

#include <elle/io/File.hh>
#include <elle/io/Path.hh>
#include <elle/io/Pattern.hh>
#include <elle/io/Piece.hh>

#include <comet/Comet.hh>

#include <elle/log.hh>

#include <elle/idiom/Close.hh>
# include <openssl/engine.h>
# include <openssl/err.h>
# include <openssl/evp.h>
# include <fcntl.h>
#include <elle/idiom/Open.hh>

ELLE_LOG_COMPONENT("elle.cryptography.KeyPair");

namespace elle
{
  namespace cryptography
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// the default key pair length.
    ///
    const Natural32             KeyPair::Default::Length = 1024;

    ///
    /// the default value for the key generation context.
    ///
    ::EVP_PKEY_CTX*             KeyPair::Contexts::Generate = nullptr;

    ///
    /// this defines a null key pair.
    ///
    const KeyPair               KeyPair::Null;

    ///
    /// this string defines the key pair files extension.
    ///
    const String                KeyPair::Extension = ".pair";

    /*---------------.
    | Static Methods |
    `---------------*/

    void
    KeyPair::initialize()
    {
      ELLE_TRACE("initializing the keypair contexts");

      // Create the context for the RSA algorithm.
      if ((KeyPair::Contexts::Generate =
           ::EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr)) == nullptr)
        throw Exception("unable to create the context");

      // Initialise the context for key generation.
      if (::EVP_PKEY_keygen_init(KeyPair::Contexts::Generate) <= 0)
        throw Exception("unable to initialise the generation context");
    }

    void
    KeyPair::clean()
    {
      ELLE_TRACE("cleaning the keypair contexts");

      // Release the generation context.
      ::EVP_PKEY_CTX_free(KeyPair::Contexts::Generate);
    }

    KeyPair
    KeyPair::generate(elle::Natural32 const length)
    {
      return (KeyPair(length));
    }

//
// ---------- construction ----------------------------------------------------
//

    KeyPair::KeyPair()
    {
      // Make sure the cryptographic system is set up.
      cryptography::setup();
    }

    KeyPair::KeyPair(elle::Natural32 const length)
    {
      // Make sure the cryptographic system is set up.
      cryptography::setup();

      // XXX[to do better with constructors]
      if (this->Generate(length) == Status::Error)
        throw Exception("unable to generate the key pair");
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method generate a RSA keypair.
    ///
    Status              KeyPair::Generate()
    {
      if (this->Generate(KeyPair::Default::Length) == Status::Error)
        escape("unable to generate the key pair");

      return Status::Ok;
    }

    ///
    /// this method generates a key pair given the parameters.
    ///
    /// the argument length represents the length of the key, in bits.
    ///
    Status              KeyPair::Generate(const Natural32       length)
    {
      struct Scope
      {
        ::EVP_PKEY* key;

        Scope() : key(nullptr) {}
        ~Scope() { ::EVP_PKEY_free(this->key); }
      } scope;

      // set the key length.
      if (::EVP_PKEY_CTX_set_rsa_keygen_bits(KeyPair::Contexts::Generate,
                                             length) <= 0)
        escape("unable to set the RSA key length %u: %s",
               length, ::ERR_error_string(ERR_get_error(), nullptr));

      // generate the EVP key.
      if (::EVP_PKEY_keygen(KeyPair::Contexts::Generate, &scope.key) <= 0)
        escape("unable to generate the key");

      assert(scope.key != nullptr);

      // create the actual public key according to the EVP structure.
      if (this->K.Create(scope.key) == Status::Error)
        escape("unable to create the public key");

      assert(this->K.key() != nullptr);

      // create the actual private key according to the EVP structure.
      if (this->k.Create(scope.key) == Status::Error)
        escape("unable to create the private key");

      assert(this->k.key() != nullptr);

      return Status::Ok;
    }

    ///
    /// this method creates a key pair from both a public and private key.
    ///
    Status              KeyPair::Create(const PublicKey&        K,
                                        const PrivateKey&       k)
    {
      // assign the attributes.
      this->K = K;
      this->k = k;

      assert(this->K.key() != nullptr);
      assert(this->k.key() != nullptr);

      return Status::Ok;
    }

    ///
    /// this method rotates a key pair based on a given seed.
    ///
    /// this seed will then be used---by people having access to it,
    /// and to the key modulus---in order to retrieve the public key.
    /// for more information, please refer to PrivateKey::Derive().
    ///
    Status              KeyPair::Rotate(const Seed&             seed,
                                        KeyPair&                kp) const
    {
      struct Scope
      {
        ::EVP_PKEY*  key;
        ::RSA*       rsa;

        Scope() : key(nullptr), rsa(nullptr) {}
        ~Scope() { ::EVP_PKEY_free(this->key); ::RSA_free(this->rsa); }
      } scope;

      // create an EVP key.
      if ((scope.key = ::EVP_PKEY_new()) == nullptr)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      // create a new RSA key.
      if ((scope.rsa = ::RSA_new()) == nullptr)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      // rotate the RSA key.
      if (comet::RSA_rotate(scope.rsa,
                            ::BN_num_bits(this->K.key()->pkey.rsa->n),
                            seed.region.contents,
                            seed.region.size) <= 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      // assign the RSA key to the EVP's.
      if (::EVP_PKEY_assign_RSA(scope.key, scope.rsa) <= 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      // stop tracking.
      scope.rsa = nullptr;

      // create the rotated public key according to the EVP structure.
      if (kp.K.Create(scope.key) == Status::Error)
        escape("unable to create the public key");

      // create the rotated private key according to the EVP structure.
      if (kp.k.Create(scope.key) == Status::Error)
        escape("unable to create the private key");

      return Status::Ok;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method check if two keypairs match.
    ///
    Boolean             KeyPair::operator==(const KeyPair&      element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return true;

      // compare the internal keys.
      if ((this->K != element.K) || (this->k != element.k))
        return false;

      return true;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the keypair internals.
    ///
    Status              KeyPair::Dump(const Natural32           margin) const
    {
      String            alignment(margin, ' ');

      std::cout << alignment << "[KeyPair]" << std::endl;

      // dump the public key.
      if (this->K.Dump(margin + 2) == Status::Error)
        escape("unable to dump the public key");

      // dump the private key.
      if (this->k.Dump(margin + 2) == Status::Error)
        escape("unable to dump the public key");

      return Status::Ok;
    }

  }
}
