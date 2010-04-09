//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/kernel/Object.hh
//
// created       julien quintard   [thu mar  5 16:04:08 2009]
// updated       julien quintard   [wed apr  7 19:14:10 2010]
//

#ifndef ETOILE_KERNEL_OBJECT_HH
#define ETOILE_KERNEL_OBJECT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/kernel/PublicKeyBlock.hh>
#include <etoile/kernel/Genre.hh>
#include <etoile/kernel/Author.hh>
#include <etoile/kernel/Size.hh>
#include <etoile/kernel/Permissions.hh>
#include <etoile/kernel/Version.hh>
#include <etoile/kernel/State.hh>
#include <etoile/kernel/Token.hh>
#include <etoile/kernel/Attributes.hh>
#include <etoile/kernel/Access.hh>
#include <etoile/kernel/Role.hh>

#include <etoile/hole/Address.hh>

namespace etoile
{
  namespace user
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// XXX
    ///
    class Agent;

  }

  namespace kernel
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
      // identifier
      //
      static const String	Name;

      //
      // constructors & destructors
      //
      Object();

      //
      // methods
      //
      Status		Create(const Genre,
			       const PublicKey&);

      Status		Update(const Author&,
			       const hole::Address&,
			       const Size&,
			       const Digest&);
      Status		Administrate(const Attributes&,
				     const hole::Address&,
				     const Permissions&,
				     const Token&);

      Status		Seal(const user::Agent&,
			     const Access* = NULL);

      Status		Validate(const hole::Address&,
				 const Access* = NULL) const;

      //
      // interfaces
      //

      // entity
      declare(Entity, Object);
      // XXX operator==

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

      Author		author;

      struct
      {
	struct
	{
	  Permissions	permissions;
	  Token		token;
	}		owner;

	Genre		genre;
	Time		stamp;

	Attributes	attributes;

	hole::Address	access;

	Version		version;
	Signature	signature;

	State		state;
      }			meta;

      struct
      {
	hole::Address	contents;

	Size		size;
	Time		stamp;

	Digest		fingerprint;

	Version		version;
	Signature	signature;

	State		state;
      }			data;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/user/Agent.hh>

#endif
