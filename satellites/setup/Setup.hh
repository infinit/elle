//
// ---------- header ----------------------------------------------------------
//
// project       setup
//
// license       infinit
//
// author        julien quintard   [sat mar 27 08:37:14 2010]
//

#ifndef SETUP_SETUP_HH
#define SETUP_SETUP_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

namespace satellite
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class implements the setup satellite.
  ///
  class Setup
  {
  public:
    //
    // enumerations
    //
    enum Operation
      {
        OperationUnknown = 0,

        OperationInitialize,
        OperationClean
      };

    //
    // static methods
    //
    static elle::Status         Initialize();
    static elle::Status         Clean();
  };

}

#endif
