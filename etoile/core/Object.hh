//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Object.hh
//
// created       julien quintard   [thu mar  5 16:04:08 2009]
// updated       julien quintard   [tue aug 11 00:43:03 2009]
//

#ifndef ETOILE_COMPONENTS_OBJECT_HH
#define ETOILE_COMPONENTS_OBJECT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/core/PublicKeyBlock.hh>
#include <etoile/core/Component.hh>
#include <etoile/core/Access.hh>
#include <etoile/core/Token.hh>
#include <etoile/core/Permissions.hh>
#include <etoile/core/Proof.hh>
#include <etoile/core/Voucher.hh>
#include <etoile/core/Contents.hh>

namespace etoile
{
  namespace core
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Object:
      public PublicKeyBlock
    {
    public:
      enum Mode
	{
	  ModeUnknown = 0,
	  ModeOwner,
	  ModeUser
	};

      //
      // constructors & destructors
      //
      Object();
      ~Object();

      //
      // methods
      //
      Status		Create(const Component,
			       const KeyPair&);

      Status		Update(const PrivateKey&,
			       const Contents&);
      // XXX other Update() for delegates and consumers

      Status		Write(const PrivateKey&,
			      const Contents&);

      Status		Administrate(const PrivateKey&,
				     const Permissions&,
				     const Address&);

      Status		Validate(const Address&) const;

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //

      struct
      {
	PublicKey	K;

	Signature	signature;
      }			owner;

      struct
      {
	Mode		mode;
	Proof*		proof;
      }			author;

      struct
      {
	Contents	contents;

	Digest		fingerprint;

	Natural32	version;
	Signature	signature;
      }			data;

      struct
      {
	struct
	{
	  Permissions	permissions;
	  Token		token;
	}		owner;

	struct
	{
	  Component	component;

	  // XXX[date last status change]
	}		status;

	Address		access;

	Natural32	version;
	Signature	signature;
      }			meta;
    };

  }
}

#endif
