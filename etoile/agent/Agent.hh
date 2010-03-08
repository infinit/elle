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
// updated       julien quintard   [fri mar  5 10:23:27 2010]
//

#ifndef ETOILE_AGENT_AGENT_HH
#define ETOILE_AGENT_AGENT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/core/Core.hh>
#include <etoile/configuration/Configuration.hh>

namespace etoile
{
  ///
  /// XXX
  ///
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
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();

      //
      // static attributes
      //
      static core::Subject	Subject;
      static KeyPair		Pair;
    };

  }
}

#endif
