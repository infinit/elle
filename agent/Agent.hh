//
// ---------- header ----------------------------------------------------------
//
// project       agent
//
// license       infinit
//
// file          /home/mycure/infinit/agent/Agent.hh
//
// created       julien quintard   [thu mar  4 17:20:28 2010]
// updated       julien quintard   [sun jun 26 14:21:29 2011]
//

#ifndef AGENT_AGENT_HH
#define AGENT_AGENT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <Infinit.hh>

#include <elle/Elle.hh>
#include <lune/Lune.hh>

namespace agent
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class implements the agent by providing an entry point
  /// to user-specific cryptographic functionalities.
  ///
  class Agent
  {
  public:
    //
    // static methods
    //
    static elle::Status		Initialize();
    static elle::Status		Clean();

    static elle::Status		Options();

    //
    // static attributes
    //
    static lune::Identity	Identity;
    static nucleus::Subject	Subject;

    static lune::Configuration	Configuration;
  };

}

#endif
