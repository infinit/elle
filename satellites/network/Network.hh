//
// ---------- header ----------------------------------------------------------
//
// project       network
//
// license       infinit
//
// author        julien quintard   [sat mar 27 08:37:14 2010]
//

#ifndef NETWORK_NETWORK_HH
#define NETWORK_NETWORK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <Infinit.hh>
#include <elle/Elle.hh>
#include <lune/Lune.hh>
#include <etoile/Etoile.hh>
#include <hole/Hole.hh>

namespace satellite
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class implements the network satellite.
  ///
  class Network
  {
  public:
    //
    // enumerations
    //
    enum Operation
      {
        OperationUnknown = 0,

        OperationCreate,
        OperationDestroy,
        OperationInformation
      };

    //
    // static methods
    //
    static elle::Status         Create(const elle::String&,
                                       const elle::String&,
                                       const hole::Model&,
                                       const elle::String&);
    static elle::Status         Destroy(const elle::String&);
    static elle::Status         Information(const elle::String&);
  };

}

#endif
