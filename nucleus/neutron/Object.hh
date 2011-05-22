//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/neutron/Object.hh
//
// created       julien quintard   [thu mar  5 16:04:08 2009]
// updated       julien quintard   [sat may 21 22:05:44 2011]
//

#ifndef NUCLEUS_NEUTRON_OBJECT_HH
#define NUCLEUS_NEUTRON_OBJECT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <nucleus/proton/Address.hh>
#include <nucleus/proton/ImprintBlock.hh>
#include <nucleus/proton/Version.hh>

#include <nucleus/neutron/Genre.hh>
#include <nucleus/neutron/Author.hh>
#include <nucleus/neutron/Size.hh>
#include <nucleus/neutron/Permissions.hh>
#include <nucleus/neutron/State.hh>
#include <nucleus/neutron/Token.hh>
#include <nucleus/neutron/Attributes.hh>
#include <nucleus/neutron/Access.hh>
#include <nucleus/neutron/Role.hh>

namespace nucleus
{
  namespace neutron
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
      public proton::ImprintBlock
    {
    public:
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
			       const proton::Address&,
			       const Size&,
			       const elle::Digest&);
      elle::Status	Administrate(const Attributes&,
				     const proton::Address&,
				     const Permissions&,
				     const Token&);

      template <typename T>
      elle::Status	Seal(const T&,
			     const Access* = NULL);

      elle::Status	Validate(const proton::Address&,
				 const Access* = NULL) const;

      //
      // operators
      //
      elle::Boolean	operator<(const Block&) const;

      //
      // interfaces
      //

      // object
      declare(Object);

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status	Serialize(elle::Archive&) const;
      elle::Status	Extract(elle::Archive&);

      //
      // attributes
      //
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

	proton::Address		access;

	proton::Version		version;
	elle::Signature		signature;

	State			state;
      }				meta;

      struct
      {
	proton::Address		contents;

	Size			size;
	elle::Time		stamp;

	elle::Digest		fingerprint;

	proton::Version		version;
	elle::Signature		signature;

	State			state;
      }				data;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <nucleus/neutron/Object.hxx>

#endif
