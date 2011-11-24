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
  const elle::Character		Component[] = "agent";

  ///
  /// the user's identity.
  ///
  lune::Identity		Agent::Identity;

  ///
  /// this variable represents the user subject.
  ///
  nucleus::Subject		Agent::Subject;

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
      // XXX everything must change!
      if (Infinit::Parser->Test("User") == elle::StatusFalse)
	{
	  name = "_______________PROTOTYPE_USER_______________";
	  // XXX name = "testdedingue";

	  // trim string
	  name = name.substr(0, name.find_first_of(' '));

	  // retrieve the identity.
	  {
	    char	idy[256];
	    char	command[256];

	    sprintf(command,
		    "mkdir -p %s/.infinit/users/%s",
		    elle::System::Path::Home.c_str(),
		    name.c_str());
	    system(command);

	    sprintf(idy, "%s/.infinit/users/%s/%s.idy",
		    elle::System::Path::Home.c_str(),
		    name.c_str(),
		    name.c_str());
	    sprintf(command,
		    "wget http://www.infinit.li/infinit/identities/%s.idy "
		    "-O %s >/dev/null 2>&1",
		    name.c_str(),
		    idy);
	    system(command);
	  }
	}
      else
	{
	  // retrieve the user name.
	  if (Infinit::Parser->Value("User", name) == elle::StatusError)
	    {
	      // display the usage.
	      Infinit::Parser->Usage();

	      escape("unable to retrieve the user name");
	    }
	}
    }

    // disable the meta logging.
    if (elle::Meta::Disable() == elle::StatusError)
      escape("unable to disable the meta logging");

    //
    // load the identity.
    //
    {
      // does the identity exist.
      if (Agent::Identity.Exist(name) == elle::StatusFalse)
	escape("the user identity does not seem to exist");

      // XXX everything must change!
      if (Infinit::Parser->Test("User") == elle::StatusFalse)
	{
	  // XXX
	  pass = "_______________PROTOTYPE_PASSWORD_______________";
	  // XXX pass = "994061d81241aef531c04953b712df423d99f933";

	  // trim string
	  pass = pass.substr(0, pass.find_first_of(' '));
	}
      else
	{
	  // prompt the user for the passphrase.
	  prompt = "Enter passphrase for keypair '" + name + "': ";

	  if (elle::Console::Input(
		pass,
		prompt,
		elle::Console::OptionPassword) == elle::StatusError)
	    escape("unable to read the input");
	}

      // load the identity.
      if (Agent::Identity.Load(name) == elle::StatusError)
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

    // nothing to do.

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
