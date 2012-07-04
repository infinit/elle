#ifndef ETOILE_GEAR_OPERATION_HH
# define ETOILE_GEAR_OPERATION_HH

namespace etoile
{
  namespace gear
  {

    ///
    /// this enumeration is used to indicate what operation an actor
    /// is currently performing on a scope.
    ///
    enum Operation
      {
        OperationUnknown,
        OperationDiscard,
        OperationStore,
        OperationDestroy
      };

  }
}

#endif
