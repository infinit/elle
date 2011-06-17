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
// updated       julien quintard   [fri jun 17 15:38:14 2011]
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
			       const Size&);
      elle::Status	Administrate(const Attributes&,
				     const proton::Address&,
				     const Permissions&,
				     const Token&);

      elle::Status	Seal(const elle::PrivateKey&,
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
	// XXX proton::Base		base;

	struct
	{
	  Permissions		permissions;
	  Token			token;
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
	// XXX proton::Base		base;

	proton::Address		contents;

	Size			size;
	elle::Time		stamp;

	proton::Version		version;
	elle::Signature		signature;

	State			state;
      }				data;
    };

  }
}

#endif
