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
// updated       julien quintard   [fri may  6 14:16:29 2011]
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
      escape("unable to dump a block without its address");

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

    // delete the block.
    delete block;

    leave();
  }

//
// ---------- functions -------------------------------------------------------
//

  ///
  /// the main function.
  ///
  Status		Main(Natural32				argc,
			     Character*				argv[])
  {
    Debug::Operation		operation;
    Parser*			parser;
    etoile::hole::Address	address;
    KeyPair			pair;
    etoile::kernel::Token	token;
    Character			option;

    enter();

    // initialize the Elle library.
    if (Elle::Initialize() == StatusError)
      escape("unable to initialize the Elle library");

    // XXX init/clean nucleus

    // set up the program.
    if (Program::Setup() == StatusError)
      escape("unable to set up the program");

    // initialize Infinit.
    if (infinit::Infinit::Initialize() == StatusError)
      escape("unable to initialize Infinit");

    // initialize the Etoile library.
    if (etoile::Etoile::Initialize() == StatusError)
      escape("unable to initialize the Etoile library");

    // initialize the operation.
    operation = Debug::OperationUnknown;

    // allocate a new parser.
    parser = new Parser(argc, argv);

    // set up the parser.
    if (parser->Register('h',
			 "help",
			 "display the help",
			 Parser::TypeNone) == StatusError)
      escape("unable to register the option");

    if (parser->Register('d',
			 "dump",
			 "dump a block identified by the address",
			 Parser::TypeNone) == StatusError)
      escape("unable to register the option");

    if (parser->Register('a',
			 "address",
			 "specifies the block's address ARG in base64",
			 Parser::TypeRequired) == StatusError)
      escape("unable to register the option");

    if (parser->Register('p',
			 "pair",
			 "specifies the key pair ARG in base64 to use "
			 "for decrypting protected information such as tokens",
			 Parser::TypeRequired) == StatusError)
      escape("unable to register the option");

    if (parser->Register('t',
			 "token",
			 "specifies the token ARG, in base64, to use for "
			 "decrypting the block's content",
			 Parser::TypeRequired) == StatusError)
      escape("unable to register the option");

    // parse.
    while (parser->Parse(option) == StatusTrue)
      {
	// act according to the current option.
	switch (option)
	  {
	  case 'd':
	    {
	      if (operation != Debug::OperationUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the dump operation cannot be set concurrently to "
			 "an other operation");
		}

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

    // trigger a command.
    switch (operation)
      {
      case Debug::OperationDump:
	{
	  // dump the block.
	  if (Debug::Dump(address, pair, token) == StatusError)
	    escape("unable to dump the block");

	  break;
	}
      default:
	{
	  // display the usage.
	  parser->Usage();

	  escape("please specify an operation to perform");
	}
      }

    // delete the parser.
    delete parser;

    // clean the Etoile library.
    if (etoile::Etoile::Clean() == StatusError)
      escape("unable to clean the Etoile library");

    // clean Infinit.
    if (infinit::Infinit::Clean() == StatusError)
      escape("unable to clean Infinit");

    // clean the Elle library.
    if (Elle::Clean() == StatusError)
      escape("unable to clean the Elle library");

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
