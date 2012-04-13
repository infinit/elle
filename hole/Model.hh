//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [fri may 20 17:46:53 2011]
//

#ifndef HOLE_MODEL_HH
#define HOLE_MODEL_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

namespace hole
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class defines the model of a network i.e its implementation.
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
        TypeSlug,
        TypeCirkle,
        TypeKool,

        Types = TypeKool,
      };

    //
    // structures
    //
    struct Descriptor
    {
      Type              type;
      elle::String      name;
    };

    //
    // constants
    //
    static const Model                  Null;
    static const Descriptor             Descriptors[Types];

    //
    // static methods
    //
    static elle::Status Convert(const elle::String&,
                                Type&);
    static elle::Status Convert(const Type,
                                elle::String&);

    //
    // constructors & destructors
    //
    Model();
    Model(const Type);

    //
    // methods
    //
    elle::Status        Create(const Type);
    elle::Status        Create(const elle::String&);

    //
    // interfaces
    //

    // object
    declare(Model);
    elle::Boolean       operator==(const Model&) const;

    // dumpable
    elle::Status        Dump(const elle::Natural32 = 0) const;

    // archivable
    elle::Status        Serialize(elle::Archive&) const;
    elle::Status        Extract(elle::Archive&);

    //
    // attributes
    //
    Type                type;
  };

}

#endif
