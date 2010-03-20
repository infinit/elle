//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Subject.hh
//
// created       julien quintard   [fri aug 14 16:26:10 2009]
// updated       julien quintard   [sat mar 20 04:08:27 2010]
//

#ifndef ETOILE_CORE_SUBJECT_HH
#define ETOILE_CORE_SUBJECT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/hole/Hole.hh>

namespace etoile
{
  namespace core
  {

//
// ---------- structures ------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Subject:
      public Entity,
      public Dumpable, public Archivable
    {
    public:
      //
      // enumeration
      //
      enum Type
	{
	  TypeUnknown,
	  TypeUser,
	  TypeGroup
	};

      //
      // constructors & destructors
      //
      Subject();
      ~Subject();

      //
      // methods
      //
      Status		Create(const PublicKey&);
      Status		Create(const hole::Address&);

      //
      // interfaces
      //

      // entity
      declare(Entity, Subject);
      Boolean		operator==(const Subject&) const;

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //
      Type		type;

      union
      {
	PublicKey*	user;
	hole::Address*	group;
      }			identifier;
    };

  }
}

#endif
