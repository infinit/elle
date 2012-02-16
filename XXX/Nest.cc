//
// ---------- header ----------------------------------------------------------
//
// project       XXX
//
// license       infinit
//
// author        julien quintard   [sat dec 24 14:08:54 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <XXX/Nest.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// XXX
    ///
    elle::Status        Nest::Register(Block*                   block)
    {
      // XXX here we register the block with an in-memory placement and no
      //     address

      // XXX loaded blocks on the other hand will be placed in memory
      //     with their original address so that future retrieval of this
      //     address are catched by the nest in order to return the newly
      //     modified version.

      return elle::StatusOk;
    }

  }
}
