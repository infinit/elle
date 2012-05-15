//
// ---------- header ----------------------------------------------------------
//
// project       agent
//
// license       infinit
//
// author        julien quintard   [thu mar  4 17:51:46 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <agent/Agent.hh>
#include <hole/Hole.hh>
#include <Infinit.hh>

namespace agent
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const elle::Character         Component[] = "agent";

  ///
  /// the user's identity.
  ///
  lune::Identity                Agent::Identity;

  ///
  /// this variable represents the user subject.
  ///
  nucleus::Subject              Agent::Subject;

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method initializes the agent.
  ///
  elle::Status          Agent::Initialize()
  {
    elle::String        prompt;
    elle::String        pass;

    // disable the meta logging.
    if (elle::Meta::Disable() == elle::StatusError)
      escape("unable to disable the meta logging");

    //
    // load the identity.
    //
    {
      // does the identity exist.
      if (Agent::Identity.Exist() == elle::StatusFalse)
        escape("the user identity does not seem to exist");

      // prompt the user for the passphrase.
      /* XXX[to change to a better version where we retrieve the passphrase from
             the watchdog]
      prompt = "Enter passphrase for keypair '" + Infinit::User + "': ";

      if (elle::Console::Input(
            pass,
            prompt,
            elle::Console::OptionPassword) == elle::StatusError)
        escape("unable to read the input");
      */
      // XXX[temporary fix]
      pass = "";

      // load the identity.
      if (Agent::Identity.Load() == elle::StatusError)
        escape("unable to load the identity");

      // verify the identity.
      if (Agent::Identity.Validate(Infinit::Authority) == elle::StatusError)
        escape("the identity seems to be invalid");

      // decrypt the identity.
      if (Agent::Identity.Decrypt(pass) == elle::StatusError)
        escape("unable to decrypt the identity");
    }

    //
    // create a subject representing the user.
    //
    {
      // create the subject.
      if (Agent::Subject.Create(Agent::Identity.pair.K) == elle::StatusError)
        escape("unable to create the user's subject");
    }

    // enable the meta logging.
    if (elle::Meta::Enable() == elle::StatusError)
      escape("unable to enable the meta logging");

    return elle::StatusOk;
  }

  ///
  /// this method cleans the agent.
  ///
  /// the components are recycled just to make sure the memory is
  /// released before the Meta allocator terminates.
  ///
  elle::Status          Agent::Clean()
  {
    // nothing to do.

    return elle::StatusOk;
  }

}
