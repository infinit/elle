//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/neutron/Subject.hh
//
// created       julien quintard   [fri aug 14 16:26:10 2009]
// updated       julien quintard   [thu may  5 13:28:42 2011]
//

#ifndef NUCLEUS_NEUTRON_SUBJECT_HH
#define NUCLEUS_NEUTRON_SUBJECT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <nucleus/proton/Address.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- structures ------------------------------------------------------
//

    ///
    /// this class is used to represents a subject i.e an entity which
    /// can be granted access such as a user or a group.
    ///
    class Subject:
      public elle::Object
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
      elle::Status	Create(const proton::Address&);

      //
      // interfaces
      //

      // object
      declare(Subject);
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
	proton::Address*	group;
      };
    };

  }
}

#endif
