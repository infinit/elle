//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/infinit/components/Entity.hh
//
// created       julien quintard   [thu mar  5 16:04:08 2009]
// updated       julien quintard   [wed mar 11 16:55:50 2009]
//

#ifndef ETOILE_COMPONENTS_ENTITY_HH
#define ETOILE_COMPONENTS_ENTITY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/components/PublicKeyBlock.hh>
#include <etoile/components/Access.hh>

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
    class Entity:
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
      Entity()
      {
	Entity::New(*this);
      }

      ~Entity()
      {
	Entity::Delete(*this);
      }

      //
      // methods
      //
      Status		Create(KeyPair&,
			       Type);

      Status		Update(PrivateKey&,
			       Address&);
      Status		Update(PrivateKey&,
			       Address&,
			       Natural32);

      Status		Manage(PrivateKey&,
			       Access::Rights,
			       Address&);

      Status		Validate(Address&);

      //
      // interfaces
      //

      // object
      static Status	New(Entity&);
      static Status	Delete(Entity&);

      Entity&		operator=(const Entity&);
      Boolean		operator==(const Entity&);
      Boolean		operator!=(const Entity&);

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
	Address		data;
	Natural32	version;

	Signature	signature;
      }			contents;

      struct
      {
	Mode		mode;
	Natural32	index;
      }			writer;

      struct
      {
	Type		type;
	Access::Rights	rights;
	Address		access;
	Natural32	version;

	Signature	signature;
      }			meta;
    };

  }
}

#endif
