//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/crypto/KeyPair.hh
//
// created       julien quintard   [sat oct 27 18:00:55 2007]
// updated       julien quintard   [mon aug  3 20:58:10 2009]
//

#ifndef ELLE_CRYPTO_KEYPAIR_HH
#define ELLE_CRYPTO_KEYPAIR_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/io/IO.hh>

#include <elle/core/Core.hh>

#include <elle/misc/Misc.hh>

#include <elle/archive/Archive.hh>

#include <elle/crypto/PublicKey.hh>
#include <elle/crypto/PrivateKey.hh>
#include <elle/crypto/Plain.hh>
#include <elle/crypto/Code.hh>
#include <elle/crypto/Clear.hh>
#include <elle/crypto/Signature.hh>

#include <openssl/engine.h>
#include <openssl/err.h>
#include <openssl/evp.h>

#include <fcntl.h>

namespace elle
{
  using namespace io;
  using namespace core;
  using namespace misc;
  using namespace archive;

  namespace crypto
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a cryptographic key pair _i.e_ a pair of public
    /// and private keys.
    ///
    /// a public key is noted with a capital 'K' while a private key is
    /// noted with a lower-case 'k'.
    ///
    class KeyPair:
      public Entity,
      public Dumpable, public Archivable
    {
    public:
      //
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();

      //
      // constants
      //
      struct Default
      {
	static const Natural32		Length;
      };

      //
      // methods
      //
      Status		Generate();
      Status		Generate(const Natural32);

      //
      // interfaces
      //

      // entity
      KeyPair&		operator=(const KeyPair&);
      Boolean		operator==(const KeyPair&) const;
      Boolean		operator!=(const KeyPair&) const;

      // dumpable
      Status		Dump(const Natural32 = 0);

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //
      PublicKey		K;
      PrivateKey	k;

      struct			Contexts
      {
	static ::EVP_PKEY_CTX*	Generate;
      };
    };

  }
}

#endif
