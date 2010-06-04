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
// updated       julien quintard   [tue may 25 11:43:12 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <agent/Agent.hh>

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
  /// this string represents the door's name to Etoile.
  ///
  const elle::String		Agent::Line("etoile");

  ///
  /// the user's identity.
  ///
  lune::Identity		Agent::Identity;

  ///
  /// the user's chain of passports.
  ///
  lune::Chain			Agent::Chain;

  ///
  /// the door to Etoile.
  ///
  elle::Door*			Agent::Channel = NULL;

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method initializes the agent, by fetching the user's key pair.
  ///
  elle::Status		Agent::Initialize(const elle::String&	name)
  {
    lune::Authority	authority;

    enter();

    //
    // load the authority.
    //
    {
      elle::PublicKey	K;

      // restore the authority's public key.
      if (K.Restore(Infinit::Authority) == elle::StatusError)
	escape("unable to restore the authority's public key");

      // create the authority based on the hard-coded public key.
      if (authority.Create(K) == elle::StatusError)
	escape("unable to create the authority");
    }

    //
    // load the user identity.
    //
    {
      elle::String	prompt;
      elle::String	pass;

      // prompt the user for the passphrase.
      prompt = "Enter passphrase for keypair '" + name + "': ";
      pass = elle::String(::getpass(prompt.c_str()));

      // load the identity.
      if (Agent::Identity.Load(name) == elle::StatusError)
	escape("unable to load the identity");

      // verify the identity.
      if (Agent::Identity.Validate(authority) != elle::StatusTrue)
	escape("the identity seems to be invalid");

      // decrypt the identity.
      if (Agent::Identity.Decrypt(pass) == elle::StatusError)
	escape("unable to decrypt the identity");
    }

    //
    // load the user's chain i.e collection of passports.
    //
    {
      if (Agent::Chain.Load(Agent::Identity) == elle::StatusError)
	escape("unable to import the user's chain of passports");
    }

    //
    // register the inward messages.
    //
    {
      elle::Callback<const elle::Report>	error(&Agent::Error);
      elle::Callback<const elle::String,
		     const elle::Code>		decrypt(&Agent::Decrypt);
      elle::Callback<const elle::String,
		     const elle::Plain>		sign(&Agent::Sign);

      // register the error message.
      if (elle::Network::Register<elle::TagError>(error) == elle::StatusError)
        escape("unable to register the error callback");

      // register the decrypt message.
      if (elle::Network::Register<TagDecrypt>(decrypt) == elle::StatusError)
        escape("unable to register the decrypt callback");

      // register the sign message.
      if (elle::Network::Register<TagSign>(sign) == elle::StatusError)
        escape("unable to register the sign callback");
    }

    //
    // trigger the Start() callback in a fiber so that it can block.
    //
    {
      elle::Entrance<>		start(&Agent::Start);
      elle::Closure<>		closure(start);

      // spawn a fiber.
      if (elle::Fiber::Spawn(closure) == elle::StatusError)
	escape("unable to spawn a fiber");
    }

    leave();
  }

  ///
  /// this method cleans the agent.
  ///
  elle::Status		Agent::Clean()
  {
    enter();

    // delete the channel, if present.
    if (Agent::Channel != NULL)
      delete Agent::Channel;

    leave();
  }

  ///
  /// this method authenticate the agent to Etoile.
  ///
  elle::Status		Agent::Authenticate()
  {
    elle::String	string;

    enter();

    //
    // connect the agent to etoile.
    //
    {
      elle::Callback<const elle::String>	error(&Agent::Error);

      // allocate a door.
      Agent::Channel = new elle::Door;

      // create the door.
      if (Agent::Channel->Create() == elle::StatusError)
	escape("unable to create the door");

      // monitor the socket.
      if (Agent::Channel->Monitor(error) == elle::StatusError)
	escape("unable to create the door");

      // connect the door.
      if (Agent::Channel->Connect(Agent::Line) == elle::StatusError)
	escape("unable to connect to Etoile");
    }

    //
    // authenticate.
    //
    {
      elle::Code		code;
      elle::Digest		digest;

      // identify to etoile by passing the user's public key for challenging.
      if (Agent::Channel->Call(
	    elle::Inputs<etoile::TagWallIdentify>(Agent::Identity.pair.K),
	    elle::Outputs<etoile::TagWallChallenge>(code)) ==
	  elle::StatusError)
	escape("unable to identify to etoile");

      // decrypt the code with the private key.
      if (Agent::Identity.pair.k.Decrypt(code, string) == elle::StatusError)
	escape("unable to decrypt the code");

      // hash the phrase string.
      if (elle::OneWay::Hash(string, digest) == elle::StatusError)
	escape("unable to hash the phrase");

      // authenticate by sending the hash of the phrase string.
      if (Agent::Channel->Call(
	    elle::Inputs<etoile::TagWallAuthenticate>(digest),
	    elle::Outputs<etoile::TagOk>()) == elle::StatusError)
	escape("unable to authenticate to etoile");
    }

    //
    // store the passphrase in the infinit configuration so that
    // applications can connect to etoile on behalf of the agent's user.
    //
    {
      lune::Phrase		phrase;

      // create the phrase.
      if (phrase.Create(string) == elle::StatusError)
	escape("unable to create the phrase");

      // store the phrase.
      if (phrase.Store() == elle::StatusError)
	escape("unable to store the phrase");
    }

    leave();
  }

//
// ---------- callbacks -------------------------------------------------------
//

  ///
  /// this callback is triggered as soon as the agent has been initialized.
  ///
  elle::Status		Agent::Start()
  {
    enter();

    // authenticate the agent.
    if (Agent::Authenticate() == elle::StatusError)
      {
	// exit the program.
	elle::Program::Exit();

	escape("unable to authenticate to etoile");
      }

    leave();
  }

  ///
  /// this callback is triggered whenever Etoile needs to decrypt
  /// a code with the agent's private key.
  ///
  elle::Status		Agent::Decrypt(const elle::String&	universe,
				       const elle::Code&	code)
  {
    lune::Passport*	passport;
    elle::Clear		clear;

    enter();

    // retrieve the universe.
    if (Agent::Chain.Lookup(universe, passport) == elle::StatusError)
      escape("unable to retrieve the passport for this universe");

    // perform the cryptographic operation.
    if (passport->pair->k.Decrypt(code, clear) == elle::StatusError)
      escape("unable to perform the decryption");

    // reply to the caller.
    if (Agent::Channel->Reply(
	  elle::Inputs<TagDecrypted>(clear)) == elle::StatusError)
      escape("unable to reply to the caller");

    leave();
  }

  ///
  /// this callback is triggered whenever Etoile needs to perform a signature.
  ///
  elle::Status		Agent::Sign(const elle::String&		universe,
				    const elle::Plain&		plain)
  {
    lune::Passport*	passport;
    elle::Signature	signature;

    enter();

    // retrieve the universe.
    if (Agent::Chain.Lookup(universe, passport) == elle::StatusError)
      escape("unable to retrieve the passport for this universe");

    // perform the cryptographic operation.
    if (passport->pair->k.Sign(plain, signature) == elle::StatusError)
      escape("unable to perform the signature");

    // reply to the caller.
    if (Agent::Channel->Reply(
	  elle::Inputs<TagSigned>(signature)) == elle::StatusError)
      escape("unable to reply to the caller");

    leave();
  }

  ///
  /// this callback is triggered whenever an error on Etoile.
  ///
  elle::Status		Agent::Error(const elle::Report&	report)
  {
    enter();

    // report the error.
    report(elle::Report::TypeError, report);

    // display the errors.
    show();

    // quit the program.
    elle::Program::Exit();

    leave();
  }

  ///
  /// this callback is triggered whenever an error occurs on the socket.
  ///
  elle::Status		Agent::Error(const elle::String&	error)
  {
    enter();

    // report the error.
    report(elle::Report::TypeError, error);

    // display the errors.
    show();

    // quit the program.
    elle::Program::Exit();

    leave();
  }

//
// ---------- functions -------------------------------------------------------
//

  ///
  /// the main function.
  ///
  elle::Status		Main(elle::Natural32			argc,
			     elle::Character*			argv[])
  {
    elle::Parser*	parser;
    elle::Character	option;
    elle::String	name;

    enter();

    // initialize the Elle library.
    if (elle::Elle::Initialize() == elle::StatusError)
      escape("unable to initialize Elle");

    // set up the program.
    if (elle::Program::Setup() == elle::StatusError)
      escape("unable to set up the program");

    // initialize the Lune library.
    if (lune::Lune::Initialize() == elle::StatusError)
      escape("unable to initialize Lune");

    // allocate a new parser.
    parser = new elle::Parser(argc, argv);

    // set up the parser.
    if (parser->Register('h',
			 "help",
			 "display the help",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('n',
			 "name",
			 "specifies the name of the user",
			 elle::Parser::TypeRequired) == elle::StatusError)
      escape("unable to register the option");

    // parse.
    while (parser->Parse(option) == elle::StatusTrue)
      {
	switch (option)
	  {
	  case 'h':
	    {
	      // display the usage.
	      parser->Usage();

	      // quit.
	      leave();
	    }
	  case 'n':
	    {
	      // retrieve the user name.
	      name.assign(optarg);

	      break;
	    }
	  case '?':
	    {
	      // display the usage.
	      parser->Usage();

	      escape("unknown option");
	    }
	  case ':':
	    {
	      // display the usage.
	      parser->Usage();

	      escape("missing argument");
	    }
	  default:
	    {
	      escape("an error occured while parsing the options");
	    }
	  }
      }

    // check the name.
    if (name.empty() == true)
      {
	// display the usage.
	parser->Usage();

	escape("please specify the name of the user");
      }

    // initialize the agent.
    if (Agent::Initialize(name) == elle::StatusError)
      escape("unable to initialize the agent");

    // launch the program.
    if (elle::Program::Launch() == elle::StatusError)
      escape("unable to process the events");

    // clean the agent.
    if (Agent::Clean() == elle::StatusError)
      escape("unable to clean the agent");

    // delete the parser.
    delete parser;

    // clean Lune
    if (lune::Lune::Clean() == elle::StatusError)
      escape("unable to clean Lune");

    // clean Elle.
    if (elle::Elle::Clean() == elle::StatusError)
      escape("unable to clean Elle");

    leave();
  }

}

//
// ---------- main ------------------------------------------------------------
//

///
/// this is the program entry point.
///
int			main(int				argc,
                             char*				argv[])
{
  agent::Main(argc, argv);

  expose();

  return (0);
}
