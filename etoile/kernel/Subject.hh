//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/kernel/Subject.hh
//
// created       julien quintard   [fri aug 14 16:26:10 2009]
// updated       julien quintard   [tue apr  6 22:44:19 2010]
//

#ifndef ETOILE_KERNEL_SUBJECT_HH
#define ETOILE_KERNEL_SUBJECT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/hole/Address.hh>

namespace etoile
{
  namespace kernel
  {

//
// ---------- structures ------------------------------------------------------
//

    ///
    /// this class is used to represents a subject i.e an entity which
    /// can be granted access such as a user or a group.
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
      Subject(const Subject&);
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
      };
    };

  }
}

#endif
