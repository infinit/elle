//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Subject.hh
//
// created       julien quintard   [fri aug 14 16:26:10 2009]
// updated       julien quintard   [mon may  3 22:56:20 2010]
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
      public elle::Object<>
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
      elle::Status	Create(const elle::PublicKey&);
      elle::Status	Create(const hole::Address&);

      //
      // interfaces
      //

      // object
      declare(Subject, _());
      elle::Boolean	operator==(const Subject&) const;

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status	Serialize(elle::Archive&) const;
      elle::Status	Extract(elle::Archive&);

      //
      // attributes
      //
      Type			type;

      union
      {
	elle::PublicKey*	user;
	hole::Address*		group;
      };
    };

  }
}

#endif
