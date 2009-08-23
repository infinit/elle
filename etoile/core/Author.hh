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
// updated       julien quintard   [sat aug 22 01:00:57 2009]
//

#ifndef ETOILE_CORE_AUTHOR_HH
#define ETOILE_CORE_AUTHOR_HH

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/core/Proof.hh>

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
	  ModeConsumer
	};

      //
      // constructors & destructors
      //
      Author();
      ~Author();

      //
      // methods
      //
      Status		Create(const Mode&);

      //
      // interfaces
      //

      // entity
      Author&		operator=(const Author&);
      Boolean		operator==(const Author&) const;
      Boolean		operator!=(const Author&) const;

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
