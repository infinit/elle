//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Object.hh
//
// created       julien quintard   [thu mar  5 16:04:08 2009]
// updated       julien quintard   [mon may  3 22:58:33 2010]
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
    /// XXX owner.subject et meta.owner.record ne sont pas serializes mais
    /// sont ici juste pour simplifier le processus de gestion des access
    ///
    class Object:
      public PublicKeyBlock
    {
    public:
      //
      // identifier
      //
      static const elle::String		Name;

      //
      // constructors & destructors
      //
      Object();

      //
      // methods
      //
      elle::Status	Create(const Genre,
			       const elle::PublicKey&);

      elle::Status	Update(const Author&,
			       const hole::Address&,
			       const Size&,
			       const elle::Digest&);
      elle::Status	Administrate(const Attributes&,
				     const hole::Address&,
				     const Permissions&,
				     const Token&);

      elle::Status	Seal(const user::Agent&,
			     const Access* = NULL);

      elle::Status	Validate(const hole::Address&,
				 const Access* = NULL) const;

      //
      // interfaces
      //

      // object
      declare(Object, _());

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status	Serialize(elle::Archive&) const;
      elle::Status	Extract(elle::Archive&);

      //
      // attributes
      //

      struct
      {
	elle::PublicKey		K;

	elle::Signature		signature;

	Subject			subject;
      }				owner;

      Author			author;

      struct
      {
	struct
	{
	  Permissions		permissions;
	  Token			token;

	  Record		record;
	}			owner;

	Genre			genre;
	elle::Time		stamp;

	Attributes		attributes;

	hole::Address		access;

	Version			version;
	elle::Signature		signature;

	State			state;
      }				meta;

      struct
      {
	hole::Address		contents;

	Size			size;
	elle::Time		stamp;

	elle::Digest		fingerprint;

	Version			version;
	elle::Signature		signature;

	State			state;
      }				data;
    };

  }
}

#endif
