//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/Model.hh
//
// created       julien quintard   [fri may 20 17:46:53 2011]
// updated       julien quintard   [mon jun 27 10:33:42 2011]
//

#ifndef HOLE_MODEL_HH
#define HOLE_MODEL_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace hole
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// XXX
  ///
  class Model:
    public elle::Object
  {
  public:
    //
    // type
    //
    enum Type
      {
	TypeUnknown = 0,

	TypeLocal,
	TypeRemote,
	TypeKool,

	Types = TypeKool,
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
    // constants
    //
    static const Model			Null;
    static const Descriptor		Descriptors[Types];

    //
    // constructors & destructors
    //
    Model();
    Model(const Type);

    //
    // methods
    //
    elle::Status	Create(const elle::String&);

    //
    // interfaces
    //

    // object
    declare(Model);
    elle::Boolean	operator==(const Model&) const;

    // dumpable
    elle::Status	Dump(const elle::Natural32 = 0) const;

    // archivable
    elle::Status	Serialize(elle::Archive&) const;
    elle::Status	Extract(elle::Archive&);

    //
    // attributes
    //
    Type		type;
  };

}

#endif
