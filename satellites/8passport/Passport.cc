//
// ---------- header ----------------------------------------------------------
//
// project       8passport
//
// license       infinit
//
// file          /home/mycure/infinit/applications/8passport/Passport.cc
//
// created       julien quintard   [thu mar  4 17:51:46 2010]
// updated       julien quintard   [tue may  4 20:26:51 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <applications/8passport/Passport.hh>

namespace application
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const Character	Component[] = "8passport";

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method creates a new passport by generating a new key pair and
  /// storing a passport block.
  ///
  elle::Status		Passport::Create(const String&		name)
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
      escape("unable to create a passport without a passport name");

    // get the file status.
    if (::stat(path.c_str(), &stat) == 0)
      escape("this passport seems to already exist");

    // prompt the passport for the passphrase.
    prompt = "Enter passphrase for keypair '" + path + "': ";
    pass = String(::getpass(prompt.c_str()));

    // generate a key pair.
    if (pair.Generate() == elle::StatusError)
      escape("unable to generate the key pair");

    // store the pair.
    if (pair.Store(path, pass) == elle::StatusError)
      escape("unable to store the key pair");

    // display the tuple passport (name, identifier).
    std::cout << name << " " << pair() << std::endl;

    leave();
  }

  ///
  /// this method destroys an existing passport.
  ///
  elle::Status		Passport::Destroy(const String&		name)
  {
    enter();

    // XXX

    leave();
  }

  ///
  /// this method sets an attribute in the passport block.
  ///
  elle::Status		Passport::Set(const String&			name,
				  const String&			attribute,
				  const String&			value)
  {
    enter();

    // XXX

    leave();
  }

  ///
  /// this method gets an attribute from the passport block.
  ///
  elle::Status		Passport::Get(const String&			name,
				  const String&			attribute)
  {
    enter();

    // XXX

    leave();
  }

  ///
  /// this method unset an attribute in the passport block.
  ///
  elle::Status		Passport::Unset(const String&		name,
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
  elle::Status		Main(Natural32				argc,
			     Character*				argv[])
  {
    Parser*		parser;
    Passport::Operation	operation;
    Character		option;
    String		name;
    String		attribute;
    String		value;

    enter();

    // initialize the Elle library.
    if (Elle::Initialize() == elle::StatusError)
      escape("unable to initialize Elle");

    // set up the program.
    if (Program::Setup() == elle::StatusError)
      escape("unable to set up the program");

    // initialize the Lune library.
    if (Lune::Initialize() == elle::StatusError)
      escape("unable to initialize Lune");

    // initialize the Etoile.
    if (Etoile::Initialize() == elle::StatusError)
      escape("unable to initialize Etoile");

    // initialize the operation.
    operation = Passport::OperationUnknown;

    // allocate a new parser.
    parser = new Parser(argc, argv);

    // set up the parser.
    if (parser->Register('h',
			 "help",
			 "display the help",
			 Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('c',
			 "create",
			 "create a new passport",
			 Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('d',
			 "destroy",
			 "destroy an existing passport",
			 Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('s',
			 "set",
			 "set an attribute in the passport's block",
			 Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('g',
			 "get",
			 "get, hence display, a passport's attribute",
			 Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('u',
			 "unset",
			 "unset an existing attribute in the passport's block",
			 Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('n',
			 "name",
			 "specifies a passport name",
			 Parser::TypeRequired) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('a',
			 "attribute",
			 "specifies an attribute name",
			 Parser::TypeRequired) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('v',
			 "value",
			 "specifies an attribute value",
			 Parser::TypeRequired) == elle::StatusError)
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
	  case 'c':
	    {
	      // check if the operation has already been set up.
	      if (operation != Passport::OperationUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the create operation cannot be set concurrently "
			 "to another operation");
		}

	      operation = Passport::OperationCreate;

	      break;
	    }
	  case 'd':
	    {
	      // check if the operation has already been set up.
	      if (operation != Passport::OperationUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the destroy operation cannot be set concurrently to "
			 "another operation");
		}

	      operation = Passport::OperationDestroy;

	      break;
	    }
	  case 's':
	    {
	      // check if the operation has already been set up.
	      if (operation != Passport::OperationUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the set operation cannot be set concurrently to "
			 "another operation");
		}

	      operation = Passport::OperationSet;

	      break;
	    }
	  case 'g':
	    {
	      // check if the operation has already been set up.
	      if (operation != Passport::OperationUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the get operation cannot be set concurrently to "
			 "another operation");
		}

	      operation = Passport::OperationGet;

	      break;
	    }
	  case 'u':
	    {
	      // check if the operation has already been set up.
	      if (operation != Passport::OperationUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the unset operation cannot be set concurrently to "
			 "another operation");
		}

	      operation = Passport::OperationUnset;

	      break;
	    }
	  case 'n':
	    {
	      // retrieve the passport name.
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
      case Passport::OperationCreate:
	{
	  // create a passport.
	  if (Passport::Create(name) == elle::StatusError)
	    escape("unable to create the passport");

	  break;
	}
      case Passport::OperationDestroy:
	{
	  // destroy a passport.
	  if (Passport::Destroy(name) == elle::StatusError)
	    escape("unable to destroy the passport");

	  break;
	}
      case Passport::OperationSet:
	{
	  // set an attribute.
	  if (Passport::Set(name, attribute, value) == elle::StatusError)
	    escape("unable to set the attribute");

	  break;
	}
      case Passport::OperationGet:
	{
	  // get an attribute
	  if (Passport::Get(name, attribute) == elle::StatusError)
	    escape("unable to get the attribute");

	  break;
	}
      case Passport::OperationUnset:
	{
	  // unset an attribute.
	  if (Passport::Unset(name, attribute) == elle::StatusError)
	    escape("unable to destroy the passport");

	  break;
	}
      case Passport::OperationUnknown:
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
    if (Etoile::Clean() == elle::StatusError)
      escape("unable to clean Etoile");

    // clean Lune
    if (Lune::Clean() == elle::StatusError)
      escape("unable to clean Lune");

    // clean Elle.
    if (Elle::Clean() == elle::StatusError)
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
