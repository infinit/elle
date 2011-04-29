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
// updated       julien quintard   [wed apr 27 11:07:16 2011]
//

#ifndef AGENT_AGENT_HH
#define AGENT_AGENT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <Infinit.hh>

#include <elle/Elle.hh>

#include <agent/Manifest.hh>

#include <elle/idiom/Close.hh>
# include <sys/stat.h>
#include <elle/idiom/Open.hh>

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
    // constants
    //
    static const elle::String		Line;

    //
    // static methods
    //
    static elle::Status		Initialize(const elle::String&);
    static elle::Status		Clean();

    static elle::Status		Authenticate();

    //
    // callbacks
    //
    static elle::Status		Start();

    static elle::Status		Decrypt(const elle::Code&);
    static elle::Status		Sign(const elle::Plain&);

    static elle::Status		Error(const elle::Report&);
    static elle::Status		Error(const elle::String&);

    //
    // static attributes
    //
    static lune::Identity	Identity;

    static elle::Door*		Channel;
  };

}

#endif
