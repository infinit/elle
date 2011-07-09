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
// updated       julien quintard   [sat jul  9 19:49:01 2011]
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

  ///
  /// this variable represents the user subject.
  ///
  nucleus::Subject		Agent::Subject;

  ///
  /// this variable contains the system configuration
  ///
  lune::Configuration		Agent::Configuration;

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method initializes the agent.
  ///
  elle::Status		Agent::Initialize()
  {
    elle::String	prompt;
    elle::String	pass;
    elle::String	name;

    enter();

    //
    // handle the parser.
    //
    {
      // retrieve the user name.
      if (Infinit::Parser->Value("User", name) == elle::StatusError)
	{
	  // display the usage.
	  Infinit::Parser->Usage();

	  escape("unable to retrieve the user name");
	}
    }

    //
    // load the identity.
    //
    {
      // does the identity exist.
      if (Agent::Identity.Exist(name) == elle::StatusFalse)
	escape("the user identity does not seem to exist");

      // prompt the user for the passphrase.
      prompt = "Enter passphrase for keypair '" + name + "': ";

      if (elle::Console::Input(
            pass,
	    prompt,
	    elle::Console::OptionPassword) == elle::StatusError)
	escape("unable to read the input");

      // load the identity.
      if (Agent::Identity.Load(name) == elle::StatusError)
	escape("unable to load the identity");

      // verify the identity.
      if (Agent::Identity.Validate(Infinit::Authority) != elle::StatusTrue)
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

    //
    // load the configuration.
    //
    {
      // load the configuration file.
      if (Agent::Configuration.Load(name) == elle::StatusError)
	escape("unable to load the configuration");

      // pull the parameters.
      if (Agent::Configuration.Pull() == elle::StatusError)
	escape("unable to pull the configuration parameters");
    }

    leave();
  }

  ///
  /// this method cleans the agent.
  ///
  /// the components are recycled just to make sure the memory is
  /// released before the Meta allocator terminates.
  ///
  elle::Status		Agent::Clean()
  {
    enter();

    // recycle the identity.
    if (Agent::Identity.Recycle<lune::Identity>() == elle::StatusError)
      escape("unable to recycle the identity");

    // recycle the subject.
    if (Agent::Subject.Recycle<nucleus::Subject>() == elle::StatusError)
      escape("unable to recycle the subject");

    // recycle the configuration.
    if (Agent::Configuration.Recycle<lune::Configuration>() ==
	elle::StatusError)
      escape("unable to recycle the configuration");

    leave();
  }

  ///
  /// this method sets up the agent-specific options.
  ///
  elle::Status		Agent::Options()
  {
    enter();

    // register the option.
    if (Infinit::Parser->Register(
	  "User",
	  'u',
	  "user",
	  "specifies the name of the user",
	  elle::Parser::KindRequired) == elle::StatusError)
      escape("unable to register the option");

    leave();
  }

}
