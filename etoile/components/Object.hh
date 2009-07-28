//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /data/mycure/repo...ries/infinit/etoile/components/Object.hh
//
// created       julien quintard   [thu mar  5 16:04:08 2009]
// updated       julien quintard   [mon jul 27 09:51:12 2009]
//

#ifndef ETOILE_COMPONENTS_OBJECT_HH
#define ETOILE_COMPONENTS_OBJECT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/components/PublicKeyBlock.hh>
#include <etoile/components/Access.hh>
#include <etoile/components/Token.hh>
#include <etoile/components/Permissions.hh>
#include <etoile/components/Proof.hh>
#include <etoile/components/Voucher.hh>

namespace etoile
{
  namespace components
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
      // constants
      //
      static const String		Class;

      //
      // constructors & destructors
      //
      Object()
      {
	Object::New(*this);
      }

      ~Object()
      {
	Object::Delete(*this);
      }

      //
      // methods
      //
      Status		Create(KeyPair&,
			       Type);

      Status		Update(PrivateKey&,
			       Address&);
      // XXX other Update() for delegates and consumers

      Status		Write(PrivateKey&,
			      Address&);

      Status		Administrate(PrivateKey&,
				     Permissions,
				     Address&);

      Status		Validate(Address&);

      //
      // interfaces
      //

      // object
      static Status	New(Object&);
      static Status	Delete(Object&);

      Object&		operator=(const Object&);
      Boolean		operator==(const Object&);
      Boolean		operator!=(const Object&);

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
