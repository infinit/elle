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
// updated       julien quintard   [mon aug  3 19:54:34 2009]
//

#ifndef ETOILE_COMPONENTS_OBJECT_HH
#define ETOILE_COMPONENTS_OBJECT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/core/PublicKeyBlock.hh>
#include <etoile/core/Access.hh>
#include <etoile/core/Token.hh>
#include <etoile/core/Permissions.hh>
#include <etoile/core/Proof.hh>
#include <etoile/core/Voucher.hh>

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
      //
      // enumerations
      //
      enum Type
	{
	  TypeUnknown = 0,
	  TypeFile,
	  TypeDirectory
	};

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
      Status		Create(const KeyPair&,
			       const Type);

      Status		Update(const PrivateKey&,
			       const Address&);
      // XXX other Update() for delegates and consumers

      Status		Write(const PrivateKey&,
			      const Address&);

      Status		Administrate(const PrivateKey&,
				     const Permissions&,
				     const Address&);

      Status		Validate(const Address&);

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0);

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
	Address		references;
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
	  Type		type;
	  Natural64	size;

	  // XXX[other information such as size, time last access/change  etc.]
	}		status;

	Address		access;

	Natural32	version;
	Signature	signature;
      }			meta;
    };

  }
}

#endif
