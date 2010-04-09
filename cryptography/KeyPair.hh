//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/cryptography/KeyPair.hh
//
// created       julien quintard   [sat oct 27 18:00:55 2007]
// updated       julien quintard   [wed mar 24 22:18:28 2010]
//

#ifndef ELLE_CRYPTOGRAPHY_KEYPAIR_HH
#define ELLE_CRYPTOGRAPHY_KEYPAIR_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/io/IO.hh>

#include <elle/core/Core.hh>

#include <elle/archive/Archive.hh>

#include <elle/miscellaneous/Status.hh>

#include <elle/cryptography/PublicKey.hh>
#include <elle/cryptography/PrivateKey.hh>
#include <elle/cryptography/Plain.hh>
#include <elle/cryptography/Code.hh>
#include <elle/cryptography/Clear.hh>
#include <elle/cryptography/Signature.hh>

#include <elle/idiom/Close.hh>
# include <openssl/engine.h>
# include <openssl/err.h>
# include <openssl/evp.h>
# include <fcntl.h>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace io;
  using namespace core;
  using namespace miscellaneous;
  using namespace archive;

  namespace cryptography
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
      struct		Default
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
      declare(Entity, KeyPair);
      Boolean		operator==(const KeyPair&) const;

      // dumpable
      Status		Dump(const Natural32 = 0) const;

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
