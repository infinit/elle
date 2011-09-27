//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit
//
// author        julien.quintard   [tue sep 27 15:59:12 2011]
//

#ifndef PIG_HANDLE_HH
#define PIG_HANDLE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/Etoile.hh>

namespace pig
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class represents a handle which is passed between the FUSE
  /// upcalls through the _info->fh_ attribute.
  ///
  class Handle
  {
  public:
    //
    // enumerations
    //
    enum Operation
      {
	OperationGetattr,
	OperationOpendir,
	OperationCreate,
	OperationOpen,
	OperationTruncate
      };

    //
    // constructors & destructors
    //
    Handle(const Operation,
	   const etoile::gear::Identifier&);
    Handle(const Operation,
	   const etoile::gear::Identifier&,
	   const nucleus::Permissions&);

    //
    // attributes
    //
    Operation			operation;
    etoile::gear::Identifier	identifier;

    union
    {
      nucleus::Permissions	permissions;
    };
  };

}

#endif
