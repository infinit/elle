//
// ---------- header ----------------------------------------------------------
//
// project       8user
//
// license       infinit
//
// file          /home/mycure/infinit/applications/8user/User.cc
//
// created       julien quintard   [thu mar  4 17:51:46 2010]
// updated       julien quintard   [sat may  1 20:58:35 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <applications/8user/User.hh>

namespace application
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const Character	Component[] = "8user";

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method creates a new user by generating a new key pair and
  /// storing a user block.
  ///
  Status		User::Create(const String&		name)
  {
    String		path =
      Lune::Keys + System::Path::Separator + name + ".pair";
    struct ::stat	stat;
    String		prompt;
    String		pass;
    KeyPair		pair;

    enter();

    // check the argument.
    if (name.empty() == true)
      escape("unable to create a user without a user name");

    // get the file status.
    if (::stat(path.c_str(), &stat) == 0)
      escape("this user seems to already exist");

    // prompt the user for the passphrase.
    prompt = "Enter passphrase for keypair '" + path + "': ";
    pass = String(::getpass(prompt.c_str()));

    // generate a key pair.
    if (pair.Generate() == StatusError)
      escape("unable to generate the key pair");

    // store the pair.
    if (pair.Store(path, pass) == StatusError)
      escape("unable to store the key pair");

    /// XXX \todo a block should be created and inserted in the hole.

    // display the tuple user (name, identifier).
    std::cout << name << " " << pair() << std::endl;

    leave();
  }

  ///
  /// this method destroys an existing user.
  ///
  Status		User::Destroy(const String&		name)
  {
    enter();

    // XXX

    leave();
  }

  ///
  /// this method sets an attribute in the user block.
  ///
  Status		User::Set(const String&			name,
				  const String&			attribute,
				  const String&			value)
  {
    enter();

    // XXX

    leave();
  }

  ///
  /// this method gets an attribute from the user block.
  ///
  Status		User::Get(const String&			name,
				  const String&			attribute)
  {
    enter();

    // XXX

    leave();
  }

  ///
  /// this method unset an attribute in the user block.
  ///
  Status		User::Unset(const String&		name,
				    const String&		attribute)
  {
    enter();

    // XXX

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
    Parser*		parser;
    User::Operation	operation;
    Character		option;
    String		name;
    String		attribute;
    String		value;

    enter();

    // initialize the Elle library.
    if (Elle::Initialize() == StatusError)
      escape("unable to initialize Elle");

    // set up the program.
    if (Program::Setup() == StatusError)
      escape("unable to set up the program");

    // initialize the Lune library.
    if (Lune::Initialize() == StatusError)
      escape("unable to initialize Lune");

    // initialize the Etoile.
    if (Etoile::Initialize() == StatusError)
      escape("unable to initialize Etoile");

    // initialize the operation.
    operation = User::OperationUnknown;

    // allocate a new parser.
    parser = new Parser(argc, argv);

    // set up the parser.
    if (parser->Register('h',
			 "help",
			 "display the help",
			 Parser::TypeNone) == StatusError)
      escape("unable to register the option");

    if (parser->Register('c',
			 "create",
			 "create a new user",
			 Parser::TypeNone) == StatusError)
      escape("unable to register the option");

    if (parser->Register('d',
			 "destroy",
			 "destroy an existing user",
			 Parser::TypeNone) == StatusError)
      escape("unable to register the option");

    if (parser->Register('s',
			 "set",
			 "set an attribute in the user's block",
			 Parser::TypeNone) == StatusError)
      escape("unable to register the option");

    if (parser->Register('g',
			 "get",
			 "get, hence display, a user's attribute",
			 Parser::TypeNone) == StatusError)
      escape("unable to register the option");

    if (parser->Register('u',
			 "unset",
			 "unset an existing attribute in the user's block",
			 Parser::TypeNone) == StatusError)
      escape("unable to register the option");

    if (parser->Register('n',
			 "name",
			 "specifies a user name",
			 Parser::TypeRequired) == StatusError)
      escape("unable to register the option");

    if (parser->Register('a',
			 "attribute",
			 "specifies an attribute name",
			 Parser::TypeRequired) == StatusError)
      escape("unable to register the option");

    if (parser->Register('v',
			 "value",
			 "specifies an attribute value",
			 Parser::TypeRequired) == StatusError)
      escape("unable to register the option");

    // parse.
    while (parser->Parse(option) == StatusTrue)
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
	  case 'c':
	    {
	      // check if the operation has already been set up.
	      if (operation != User::OperationUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the create operation cannot be set concurrently "
			 "to another operation");
		}

	      operation = User::OperationCreate;

	      break;
	    }
	  case 'd':
	    {
	      // check if the operation has already been set up.
	      if (operation != User::OperationUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the destroy operation cannot be set concurrently to "
			 "another operation");
		}

	      operation = User::OperationDestroy;

	      break;
	    }
	  case 's':
	    {
	      // check if the operation has already been set up.
	      if (operation != User::OperationUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the set operation cannot be set concurrently to "
			 "another operation");
		}

	      operation = User::OperationSet;

	      break;
	    }
	  case 'g':
	    {
	      // check if the operation has already been set up.
	      if (operation != User::OperationUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the get operation cannot be set concurrently to "
			 "another operation");
		}

	      operation = User::OperationGet;

	      break;
	    }
	  case 'u':
	    {
	      // check if the operation has already been set up.
	      if (operation != User::OperationUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the unset operation cannot be set concurrently to "
			 "another operation");
		}

	      operation = User::OperationUnset;

	      break;
	    }
	  case 'n':
	    {
	      // retrieve the user name.
	      name.assign(optarg);

	      break;
	    }
	  case 'a':
	    {
	      // retrieve the attribute name.
	      attribute.assign(optarg);

	      break;
	    }
	  case 'v':
	    {
	      // retrieve the value.
	      value.assign(optarg);

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

    // trigger the operation.
    switch (operation)
      {
      case User::OperationCreate:
	{
	  // create a user.
	  if (User::Create(name) == StatusError)
	    escape("unable to create the user");

	  break;
	}
      case User::OperationDestroy:
	{
	  // destroy a user.
	  if (User::Destroy(name) == StatusError)
	    escape("unable to destroy the user");

	  break;
	}
      case User::OperationSet:
	{
	  // set an attribute.
	  if (User::Set(name, attribute, value) == StatusError)
	    escape("unable to set the attribute");

	  break;
	}
      case User::OperationGet:
	{
	  // get an attribute
	  if (User::Get(name, attribute) == StatusError)
	    escape("unable to get the attribute");

	  break;
	}
      case User::OperationUnset:
	{
	  // unset an attribute.
	  if (User::Unset(name, attribute) == StatusError)
	    escape("unable to destroy the user");

	  break;
	}
      case User::OperationUnknown:
      default:
	{
	  // display the usage.
	  parser->Usage();

	  escape("please specify an operation to perform");
	}
      }

    // delete the parser.
    delete parser;

    // clean the Etoile.
    if (Etoile::Clean() == StatusError)
      escape("unable to clean Etoile");

    // clean Lune
    if (Lune::Clean() == StatusError)
      escape("unable to clean Lune");

    // clean Elle.
    if (Elle::Clean() == StatusError)
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
                             char**				argv)
{
  application::Main(argc, argv);

  expose();

  return (0);
}
