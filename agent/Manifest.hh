//
// ---------- header ----------------------------------------------------------
//
// project       agent
//
// license       infinit
//
// file          /home/mycure/infinit/agent/Manifest.hh
//
// created       julien quintard   [thu mar  4 17:34:08 2010]
// updated       julien quintard   [thu mar  4 18:35:43 2010]
//

#ifndef AGENT_MANIFEST_HH
#define AGENT_MANIFEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <agent/Agent.hh>

//
// ---------- tags ------------------------------------------------------------
//

namespace agent
{

  //
  // enumerations
  //
  enum Tag
    {
      /// XXX \todo trouver un truc plus propre pour eviter les collisions
      TagNone = 1000,

      TagDecrypt,
      TagSign,

      Tags = TagSign + 1
    };

}

//
// ---------- manifest --------------------------------------------------------
//

Inward(::agent::Agent::TagDecrypt,
       Parameters(::elle::crypto::Code));
Outward(::agent::Agent:::Interface::TagUserIdentify,
        Parameters(::elle::crypto::Code));

#endif
