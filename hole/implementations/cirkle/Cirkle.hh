//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [fri aug 12 13:49:49 2011]
//

#ifndef HOLE_IMPLEMENTATIONS_CIRKLE_CIRKLE_HH
#define HOLE_IMPLEMENTATIONS_CIRKLE_CIRKLE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <hole/implementations/cirkle/Machine.hh>

namespace hole
{
  namespace implementations
  {
    ///
    /// XXX
    ///
    namespace cirkle
    {

//
// ---------- classes ---------------------------------------------------------
//

      ///
      /// XXX
      ///
      ///
      class Cirkle
      {
      public:
        //
        // static attributes
        //
        static Machine*         Computer;
      };

    }
  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <hole/implementations/cirkle/Cluster.hh>
#include <hole/implementations/cirkle/Implementation.hh>
#include <hole/implementations/cirkle/Machine.hh>
#include <hole/implementations/cirkle/Manifest.hh>
#include <hole/implementations/cirkle/Neighbour.hh>
#include <hole/implementations/cirkle/Neighbourhood.hh>
#include <hole/implementations/cirkle/RoutingTable.hh>

#endif
