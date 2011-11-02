//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [fri aug 14 16:26:10 2009]
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
	  TypeGroup,

	  Types
	};

      //
      // structures
      //
      struct Descriptor
      {
	Type		type;
	elle::String	name;
      };

      //
      // static methods
      //
      static elle::Status	Convert(const elle::String&,
					Type&);
      static elle::Status	Convert(const Type,
					elle::String&);

      //
      // static attributes
      //
      static const Descriptor		Descriptors[Types];

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
