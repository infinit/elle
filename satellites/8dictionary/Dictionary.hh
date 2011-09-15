//
// ---------- header ----------------------------------------------------------
//
// project       8dictionary
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

#include <Infinit.hh>
#include <elle/Elle.hh>
#include <lune/Lune.hh>
#include <etoile/Etoile.hh>

namespace application
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class implements the 8dictionary application.
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
    static elle::Status		Add(const elle::String&,
				    const Type&,
				    const elle::String&,
				    const elle::Unique&);
    static elle::Status		Remove(const elle::String&,
				       const Type&,
				       const elle::String&);
    static elle::Status		Dump(const elle::String&,
				     const Type&,
				     const elle::String&);
    static elle::Status		Show(const elle::String&,
				     const Type&);
  };

}

#endif
