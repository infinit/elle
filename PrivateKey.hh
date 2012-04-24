//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [tue oct 30 10:02:18 2007]
//

#ifndef ELLE_CRYPTOGRAPHY_PRIVATEKEY_HH
#define ELLE_CRYPTOGRAPHY_PRIVATEKEY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

#include <elle/radix/Object.hh>

#include <elle/cryptography/Plain.hh>
#include <elle/cryptography/Code.hh>
#include <elle/cryptography/Signature.hh>
#include <elle/cryptography/Clear.hh>
#include <elle/cryptography/Seed.hh>
#include <elle/utility/Buffer.hh>

// XXX remove openssl include from headers
#include <elle/idiom/Close.hh>
# include <openssl/rsa.h>
# include <openssl/engine.h>
# include <openssl/bn.h>
# include <openssl/crypto.h>
# include <openssl/err.h>
# include <openssl/evp.h>
#include <elle/idiom/Open.hh>

namespace elle
{

  namespace cryptography
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// the Seed must be forward declared to prevent conflicts.
    ///
    class Seed;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a private key based on the RSA cryptosystem.
    ///
    /// note that the cryptographic methods are set as virtual because
    /// some classes may wish to override it, as it is the case in the
    /// Etoile's agent.
    ///
    class PrivateKey:
      public Object
    {
    public:
      //
      // constants
      //
      static const PrivateKey           Null;

      //
      // constructors & destructors
      //
      PrivateKey();
      PrivateKey(const PrivateKey&);
      ~PrivateKey();

      //
      // methods
      //
      Status            Create(const ::EVP_PKEY*);
      Status            Create(Large*,
                               Large*,
                               Large*,
                               Large*,
                               Large*,
                               Large*,
                               Large*,
                               Large*);

    public:
      // Decrypt methods
      Status
      Decrypt(elle::utility::WeakBuffer const& in, Clear& out) const;

      Status
      Decrypt(elle::standalone::Region const& in, Clear& out) const;

      Status
      Decrypt(elle::cryptography::Code const& in, Clear& out) const;

      template<typename T> Status
      Decrypt(T const& in, Clear& out) const;


    public:
      // Encrypt methods
      Status
      Encrypt(Region const& in,  Code& out) const;

      Status
      Encrypt(elle::utility::WeakBuffer const& in,  Code& out) const;

      template<typename T> Status
      Encrypt(T const& in, Code& out) const;

    public:
      // Sign methods
      Status
      Sign(elle::utility::WeakBuffer const& in, Signature& out) const;

      Status
      Sign(Region const& in, Signature& out) const;

      template<typename T> Status
      Sign(T const& in, Signature& out) const;


    public:
      Status
      Derive(const Seed& seed, PublicKey& key) const;

      //
      // interfaces
      //

      // object
      declare(PrivateKey);
      Boolean           operator==(const PrivateKey&) const;

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      // archivable
      //Status            Serialize(Archive&) const;
      //Status            Extract(Archive&);

      //
      // attributes
      //
    private:
      ::EVP_PKEY*       _key;
      struct
      {
        ::EVP_PKEY_CTX* decrypt;
        ::EVP_PKEY_CTX* sign;
        ::EVP_PKEY_CTX* encrypt;
      }                 _contexts;

    public:
      ::EVP_PKEY const* key() const { return this->_key; }

    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/cryptography/PrivateKey.hxx>

#endif
