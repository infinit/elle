//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/agent/Agent.cc
//
// created       julien quintard   [mon aug 17 01:20:54 2009]
// updated       julien quintard   [fri mar  5 10:41:57 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/agent/Agent.hh>

namespace etoile
{
  namespace agent
  {

//
// ---------- definitions -----------------------------------------------------
//

    // XXX A CHANGER!!!
    KeyPair		Agent::Pair;
    core::Subject	Agent::Subject;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes the agent.
    ///
    Status		Agent::Initialize()
    {
      enter();

      // XXX

      leave();
    }

    ///
    /// this method cleans the agent.
    ///
    Status		Agent::Clean()
    {
      enter();

      // XXX

      leave();
    }

  }
}
