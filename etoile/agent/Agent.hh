//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/agent/Agent.hh
//
// created       julien quintard   [mon aug 17 01:12:15 2009]
// updated       julien quintard   [mon aug 17 02:09:19 2009]
//

#ifndef ETOILE_AGENT_AGENT_HH
#define ETOILE_AGENT_AGENT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/core/Core.hh>

namespace etoile
{
  namespace agent
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class reprensents a cryptographic agent which is called
    /// whenever a cryptographic operation needs to be performed on behalf
    /// of the user.
    ///
    class Agent
    {
    public:
      //
      // attributes
      //

      // XXX[temporary since no external agent yet]
      static core::Subject	Subject;
      static KeyPair		Pair;
    };

  }
}

#endif
