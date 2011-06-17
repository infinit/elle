//
// ---------- header ----------------------------------------------------------
//
// project       agent
//
// license       infinit
//
// file          /home/mycure/infinit/agent/Agent.cc
//
// created       julien quintard   [thu mar  4 17:51:46 2010]
// updated       julien quintard   [fri jun 17 16:40:00 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <agent/Agent.hh>

#include <Infinit.hh>

namespace agent
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const elle::Character		Component[] = "agent";

  ///
  /// this user's identity.
  ///
  lune::Identity		Agent::Identity;

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method initializes the agent.
  ///
  elle::Status		Agent::Initialize(const elle::String&	name)
  {
    elle::String	prompt;
    elle::String	pass;

    enter();

    // does the identity exist.
    if (Agent::Identity.Exist(name) == elle::StatusFalse)
      escape("the user identity does not seem to exist");

    // prompt the user for the passphrase.
    prompt = "Enter passphrase for keypair '" + name + "': ";
    pass = elle::String(::getpass(prompt.c_str()));

    // load the identity.
    if (Agent::Identity.Load(name) == elle::StatusError)
      escape("unable to load the identity");

    // verify the identity.
    if (Agent::Identity.Validate(Infinit::Authority) != elle::StatusTrue)
      escape("the identity seems to be invalid");

    // decrypt the identity.
    if (Agent::Identity.Decrypt(pass) == elle::StatusError)
      escape("unable to decrypt the identity");

    leave();
  }

  ///
  /// this method cleans the agent.
  ///
  elle::Status		Agent::Clean()
  {
    enter();

    // nothing to do.

    leave();
  }

}
