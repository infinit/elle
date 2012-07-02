//
// ---------- header ----------------------------------------------------------
//
// project       dictionary
//
// license       infinit
//
// author        julien quintard   [sat mar 27 08:37:14 2010]
//

#ifndef DICTIONARY_DICTIONARY_HH
#define DICTIONARY_DICTIONARY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>
#include <elle/io/fwd.hh>

namespace satellite
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class implements the dictionary satellite.
  ///
  class Dictionary
  {
  public:
    //
    // enumerations
    //
    enum Operation
      {
        OperationUnknown = 0,

        OperationAdd,
        OperationRemove,
        OperationDump,
        OperationShow
      };

    enum Type
      {
        TypeUnknown = 0,

        TypeUser,
        TypeGroup
      };

    //
    // static methods
    //
    static elle::Status         Add(elle::String const&,
                                    const Type&,
                                    const elle::String&,
                                    const elle::io::Unique&);
    static elle::Status         Remove(elle::String const&,
                                       const Type&,
                                       const elle::String&);
    static elle::Status         Dump(elle::String const&,
                                     const Type&,
                                     const elle::String&);
    static elle::Status         Show(elle::String const&,
                                     const Type&);
  };

}

#endif
