//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Author.hh
//
// created       julien quintard   [fri aug 21 22:04:45 2009]
// updated       julien quintard   [mon mar  8 23:11:37 2010]
//

#ifndef ETOILE_CORE_AUTHOR_HH
#define ETOILE_CORE_AUTHOR_HH

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/core/Proof.hh>
#include <etoile/core/Voucher.hh>

namespace etoile
{
  namespace core
  {

//
// ---------- classes ---------------------------------------------------------
//

    class Author:
      public Entity,
      public Dumpable, public Archivable
    {
    public:
      //
      // enumerations
      //
      enum Mode
	{
	  ModeUnknown = 0,
	  ModeOwner,
	  ModeDelegate,
	  ModeVassal
	};

      //
      // constructors & destructors
      //
      Author();
      ~Author();

      //
      // methods
      //
      Status		Create();
      Status		Create(const Natural32&);
      Status		Create(const Natural32&,
			       const Voucher&);

      //
      // interfaces
      //

      // entity
      embed(Entity, Author);
      Boolean		operator==(const Author&) const;

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //
      Mode		mode;
      Proof*		proof;
    };

  }
}

#endif
