//
// ---------- header ----------------------------------------------------------
//
// project       8debug
//
// license       infinit
//
// file          /home/mycure/infinit/applications/8debug/Debug.cc
//
// created       julien quintard   [thu mar  4 17:51:46 2010]
// updated       julien quintard   [fri apr 23 13:13:30 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <applications/8debug/Debug.hh>

namespace application
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const Character	Component[] = "8debug";

//
// ---------- functions -------------------------------------------------------
//

  ///
  /// this function displays the usage.
  ///
  Void			Usage(const String&			text)
  {
    // display the usage.
    std::cerr << "$> 8debug -d "
	      << "-a {block address} -p {key pair} -t {token}"
	      << std::endl;

    // display the text.
    std::cout << std::endl << "[error] " << text << std::endl;

    ::exit(EXIT_FAILURE);
  }

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method dump a block.
  ///
  Status		Debug::Dump(const etoile::hole::Address& address,
				    const KeyPair&		pair,
				    const etoile::kernel::Token& token)
  {
    etoile::hole::Block*	block;

    enter();

    // valid the arguments.
    if (address == etoile::hole::Address::Null)
      Usage("unable to dump a block without its address");

    // retrieve the block directly from the hole.
    if (etoile::hole::Hole::Get(address, block) != StatusTrue)
      escape("unable to get the block");

    // perform actions depending on the block type.
    switch (block->component)
      {
      case etoile::hole::ComponentData:
	{
	  etoile::kernel::Contents<etoile::kernel::Data>*	contents =
	    static_cast<etoile::kernel::Contents<etoile::kernel::Data>*>
	      (block);
	  etoile::user::Agent					agent;
	  SecretKey						key;

	  // check the arguments.
	  if ((pair != KeyPair::Null) &&
	      (token != etoile::kernel::Token::Null))
	    {
	      // create a local agent.
	      if (agent.Create(pair) == StatusError)
		escape("unable to create the agent");

	      // extract the secret key from the token.
	      if (token.Extract(agent, key) == StatusError)
		escape("unable to extract the secret key from the token");

	      // decrypt the contents.
	      if (contents->Decrypt(key) == StatusError)
		escape("unable to decrypt the contents");
	    }

	  break;
	}
      case etoile::hole::ComponentCatalog:
	{
	  etoile::kernel::Contents<etoile::kernel::Catalog>*	contents =
	    static_cast<etoile::kernel::Contents<etoile::kernel::Catalog>*>
	      (block);
	  etoile::user::Agent					agent;
	  SecretKey						key;

	  // check the arguments.
	  if ((pair != KeyPair::Null) &&
	      (token != etoile::kernel::Token::Null))
	    {
	      // create a local agent.
	      if (agent.Create(pair) == StatusError)
		escape("unable to create the agent");

	      // extract the secret key from the token.
	      if (token.Extract(agent, key) == StatusError)
		escape("unable to extract the secret key from the token");

	      // decrypt the contents.
	      if (contents->Decrypt(key) == StatusError)
		escape("unable to decrypt the contents");
	    }

	  break;
	}
      case etoile::hole::ComponentReference:
	{
	  etoile::kernel::Contents<etoile::kernel::Reference>*	contents =
	    static_cast<etoile::kernel::Contents<etoile::kernel::Reference>*>
	      (block);
	  etoile::user::Agent					agent;
	  SecretKey						key;

	  // check the arguments.
	  if ((pair != KeyPair::Null) &&
	      (token != etoile::kernel::Token::Null))
	    {
	      // create a local agent.
	      if (agent.Create(pair) == StatusError)
		escape("unable to create the agent");

	      // extract the secret key from the token.
	      if (token.Extract(agent, key) == StatusError)
		escape("unable to extract the secret key from the token");

	      // decrypt the contents.
	      if (contents->Decrypt(key) == StatusError)
		escape("unable to decrypt the contents");
	    }

	  break;
	}
      }

    // dump the block.
    if (block->Dump() == StatusError)
      escape("unable to dump the block");

    leave();
  }

//
// ---------- functions -------------------------------------------------------
//

  ///
  /// this method initializes the application.
  ///
  Status		Debug::Initialize(const Natural32	argc,
					  const Character*	argv[])
  {
    etoile::hole::Address	root;
    String			path;

    enter();

    // initialize the Elle library.
    if (Elle::Initialize() == StatusError)
      escape("unable to initialize the Elle library");

    // set up the program.
    if (Program::Setup(argc, argv) == StatusError)
      escape("unable to set up the program");

    // initialize the Etoile library.
    if (etoile::Etoile::Initialize() == StatusError)
      escape("unable to initialize the Etoile library");

    leave();
  }

  ///
  /// this method cleans the application.
  ///
  Status		Debug::Clean()
  {
    enter();
 
    // clean the Etoile library.
    if (etoile::Etoile::Clean() == StatusError)
      escape("unable to clean the Etoile library");

    // clean the Elle library.
    if (Elle::Clean() == StatusError)
      escape("unable to clean the Elle library");

    leave();
  }

  ///
  /// the main function.
  ///
  Status		Main(Natural32				argc,
			     Character*				argv[])
  {
    Debug::Operation		operation = Debug::OperationUnknown;
    etoile::hole::Address	address;
    KeyPair			pair;
    etoile::kernel::Token	token;
    int				opt;

    enter();

    // initialize the application.
    if (Debug::Initialize(argc, (const Character**)argv) == StatusError)
      escape("unable to initialize the application");

    // parse the options.
    while ((opt = ::getopt(argc, argv, "da:p:t:")) != -1)
      {
	// act according to the current option.
	switch (opt)
	  {
	  case 'd':
	    {
	      if (operation != Debug::OperationUnknown)
		Usage("the dump operation cannot be set concurrently to "
		      "an other operation");

	      operation = Debug::OperationDump;

	      break;
	    }
	  case 'a':
	    {
	      String		string;

	      // save the argument in a string.
	      string.assign(optarg);

	      // decode the base64 string.
	      if (Base64::Decode(string, address) == StatusError)
		escape("unable to decode the address argument");

	      break;
	    }
	  case 'p':
	    {
	      String		string;

	      // save the argument in a string.
	      string.assign(optarg);

	      // decode the base64 string.
	      if (Base64::Decode(string, pair) == StatusError)
		escape("unable to decode the pair");

	      break;
	    }
	  case 't':
	    {
	      String		string;

	      // save the argument in a string.
	      string.assign(optarg);

	      // decode the base64 string.
	      if (Base64::Decode(string, token) == StatusError)
		escape("unable to decode the token");

	      break;
	    }
	  default:
	    {
	      Usage("unknown option");

	      break;
	    }
	  }
      }

    // trigger a command.
    switch (operation)
      {
      case Debug::OperationDump:
	{
	  if (Debug::Dump(address, pair, token) == StatusError)
	    escape("unable to dump the block");

	  break;
	}
      default:
	{
	  Usage("no operation has been requested");

	  break;
	}
      }

    // clean the application.
    if (Debug::Clean() == StatusError)
      escape("unable to clean the application");

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
                             char**				argv)
{
  application::Main(argc, argv);

  expose();

  return (0);
}
