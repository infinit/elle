#include <cryptography/KeyPair.hh>
#include <cryptography/KeyPair.hh>
#include <cryptography/Cipher.hh>
#include <cryptography/SecretKey.hh>
#include <cryptography/Seed.hh>
#include <cryptography/cryptography.hh>

#include <elle/types.hh>
#include <elle/Exception.hh>

#include <comet/Comet.hh>

#include <elle/log.hh>

#include <elle/idiom/Close.hh>
# include <openssl/engine.h>
# include <openssl/err.h>
# include <openssl/evp.h>
# include <fcntl.h>
#include <elle/idiom/Open.hh>

ELLE_LOG_COMPONENT("infinit.cryptography.KeyPair");

namespace infinit
{
  namespace cryptography
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// the default key pair length.
    ///
    const elle::Natural32             KeyPair::Default::Length = 1024;

    ///
    /// the default value for the key generation context.
    ///
    ::EVP_PKEY_CTX*             KeyPair::Contexts::Generate = nullptr;

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
        throw elle::Exception("unable to create the context"); // XXX[err string]

      // Initialise the context for key generation.
      if (::EVP_PKEY_keygen_init(KeyPair::Contexts::Generate) <= 0)
        throw elle::Exception("unable to initialise the generation context"); // XXX[err string]
    }

    void
    KeyPair::clean()
    {
      ELLE_TRACE("cleaning the keypair contexts");

      // Release the generation context.
      ::EVP_PKEY_CTX_free(KeyPair::Contexts::Generate);
    }

    KeyPair const&
    KeyPair::null()
    {
      static KeyPair kp; // XXX[to re-work so as to have a null type]

      return (kp);
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
      cryptography::require();
    }

    KeyPair::KeyPair(elle::Natural32 const length)
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();

      // XXX[to improve so as to call the K/k constructors]

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
      if (this->K.Create(scope.key) == elle::Status::Error)
        escape("unable to create the public key");

      assert(this->K.key() != nullptr);

      // create the actual private key according to the EVP structure.
      if (this->k.Create(scope.key) == elle::Status::Error)
        escape("unable to create the private key");

      assert(this->k.key() != nullptr);
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method rotates a key pair based on a given seed.
    ///
    /// this seed will then be used---by people having access to it,
    /// and to the key modulus---in order to retrieve the public key.
    /// for more information, please refer to PrivateKey::Derive().
    ///
    elle::Status              KeyPair::Rotate(const Seed&             seed,
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
      if (kp.K.Create(scope.key) == elle::Status::Error)
        escape("unable to create the public key");

      // create the rotated private key according to the EVP structure.
      if (kp.k.Create(scope.key) == elle::Status::Error)
        escape("unable to create the private key");

      return elle::Status::Ok;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method check if two keypairs match.
    ///
    elle::Boolean             KeyPair::operator==(const KeyPair&      element) const
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
    elle::Status              KeyPair::Dump(const elle::Natural32           margin) const
    {
      elle::String            alignment(margin, ' ');

      std::cout << alignment << "[KeyPair]" << std::endl;

      // dump the public key.
      if (this->K.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the public key");

      // dump the private key.
      if (this->k.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the public key");

      return elle::Status::Ok;
    }

  }
}
