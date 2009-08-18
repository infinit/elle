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
// updated       julien quintard   [mon aug 17 00:46:46 2009]
//

#ifndef ETOILE_CORE_OBJECT_HH
#define ETOILE_CORE_OBJECT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/core/PublicKeyBlock.hh>
#include <etoile/core/Genre.hh>
#include <etoile/core/Access.hh>
#include <etoile/core/Token.hh>
#include <etoile/core/Permissions.hh>
#include <etoile/core/Proof.hh>
#include <etoile/core/Voucher.hh>
#include <etoile/core/Version.hh>
#include <etoile/core/Catalog.hh>
#include <etoile/core/Data.hh>
#include <etoile/core/Reference.hh>
#include <etoile/core/Contents.hh>

#include <etoile/hole/Hole.hh>

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
      Status		Create(const Genre,
			       const PublicKey&);

      Status		Update(const PrivateKey&);
      Status		Administrate(const PrivateKey&);

      Status		Validate(const hole::Address&) const;

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
	struct
	{
	  Permissions	permissions;
	  Token		token;
	}		owner;

	struct
	{
	  Genre		genre;

	  // XXX[date last status change]
	}		status;

	hole::Address	access;

	Version		version;
	Signature	signature;
      }			meta;

      struct
      {
	Contents	contents;

	Digest		fingerprint;

	Version		version;
	Signature	signature;
      }			data;
    };

  }
}

#endif
