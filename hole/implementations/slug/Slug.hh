//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [fri aug 12 13:49:49 2011]
//

#ifndef HOLE_IMPLEMENTATIONS_SLUG_SLUG_HH
#define HOLE_IMPLEMENTATIONS_SLUG_SLUG_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <hole/implementations/slug/Machine.hh>

namespace hole
{
  namespace implementations
  {
    ///
    /// XXX
    ///
    namespace slug
    {

//
// ---------- classes ---------------------------------------------------------
//

      ///
      /// XXX
      ///
      ///
      class Slug
      {
      public:
    //
    // static attributes
    //
    static Machine*		Computer;
      };

    }
  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <hole/implementations/slug/Cluster.hh>
#include <hole/implementations/slug/Implementation.hh>
#include <hole/implementations/slug/Machine.hh>
#include <hole/implementations/slug/Manifest.hh>
#include <hole/implementations/slug/Neighbour.hh>
#include <hole/implementations/slug/Neighbourhood.hh>
#include <hole/implementations/slug/RoutingTable.hh>

#endif
