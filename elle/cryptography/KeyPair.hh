//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/cryptography/KeyPair.hh
//
// created       julien quintard   [sat oct 27 18:00:55 2007]
// updated       julien quintard   [mon may  3 22:32:13 2010]
//

#ifndef ELLE_CRYPTOGRAPHY_KEYPAIR_HH
#define ELLE_CRYPTOGRAPHY_KEYPAIR_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>
#include <elle/core/Boolean.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Object.hh>

#include <elle/archive/Archive.hh>

#include <elle/cryptography/PublicKey.hh>
#include <elle/cryptography/PrivateKey.hh>

namespace elle
{
  using namespace core;
  using namespace archive;
  using namespace io;

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
      public Object<FormatBase64, FormatCustom>
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

      static const KeyPair		Null;

      //
      // methods
      //
      Status		Generate();
      Status		Generate(const Natural32);

      //
      // interfaces
      //

      // object
      declare(KeyPair, _(FormatBase64, FormatCustom));
      Boolean		operator==(const KeyPair&) const;

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      // fileable
      Status		Load(const String&,
			     const String&);
      Status		Store(const String&,
			      const String&) const;

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
