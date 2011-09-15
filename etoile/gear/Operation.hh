//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [fri jun 10 14:28:17 2011]
//

#ifndef ETOILE_GEAR_OPERATION_HH
#define ETOILE_GEAR_OPERATION_HH

namespace etoile
{
  namespace gear
  {

//
// ---------- enumerations ----------------------------------------------------
//

    ///
    /// this enumeration is used to indicate what operation an actor
    /// is currently performing on a scope.
    ///
    enum Operation
      {
	OperationDiscard,
	OperationStore,
	OperationDestroy
      };

  }
}

#endif
