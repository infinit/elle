#ifndef NETWORK_NETWORK_HH
#define NETWORK_NETWORK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>
#include <hole/fwd.hh>
#include <horizon/fwd.hh>

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
                                       hole::Openness const& openness,
                                       horizon::Policy const& policy,
                                       const elle::String&);
    static elle::Status         Destroy(const elle::String&);
    static elle::Status         Information(const elle::String&);
  };

}

#endif
