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
// updated       julien quintard   [fri may  6 14:30:01 2011]
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
  const elle::Character		Component[] = "8user";

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method creates a new user by generating a new key pair and
  /// storing a user block.
  ///
  elle::Status		User::Create(const elle::String&	name)
  {
    elle::String	prompt;
    elle::String	pass;
    elle::KeyPair	pair;
    lune::Authority	authority;
    lune::Identity	identity;
    lune::Dictionary	dictionary;

    enter();

    // check the argument.
    if (name.empty() == true)
      escape("unable to create a user without a user name");

    // check if the user already exists.
    if (identity.Exist(name) == elle::StatusTrue)
      escape("this user seems to already exist");

    // check if the authority exists.
    if (authority.Exist() == elle::StatusFalse)
      escape("unable to locate the authority file");

    // prompt the user for the passphrase.
    prompt = "Enter passphrase for the authority: ";
    pass = elle::String(::getpass(prompt.c_str()));

    // load the authority.
    if (authority.Load() == elle::StatusError)
      escape("unable to load the authority");

    // decrypt the authority.
    if (authority.Decrypt(pass) == elle::StatusError)
      escape("unable to decrypt the authority");

    // prompt the user for the passphrase.
    prompt = "Enter passphrase for keypair '" + name + "': ";
    pass = elle::String(::getpass(prompt.c_str()));

    // generate a key pair.
    if (pair.Generate() == elle::StatusError)
      escape("unable to generate the key pair");

    // create the identity.
    if (identity.Create(name, pair) == elle::StatusError)
      escape("unable to create the identity");

    // encrypt the identity.
    if (identity.Encrypt(pass) == elle::StatusError)
      escape("unable to encrypt the identity");

    // seal the identity.
    if (identity.Seal(authority) == elle::StatusError)
      escape("unable to seal the identity");

    // store the identity.
    if (identity.Store(name) == elle::StatusError)
      escape("unable to store the identity");

    // store an empty dictionary.
    if (dictionary.Store(name) == elle::StatusError)
      escape("unable to store the dictionary");

    leave();
  }

  ///
  /// this method destroys an existing user.
  ///
  elle::Status		User::Destroy(const elle::String&	name)
  {
    enter();

    //
    // remove the identity.
    //
    {
      lune::Identity	identity;

      // check the argument.
      if (name.empty() == true)
	escape("unable to create a user without a user name");

      // check if the user already exists.
      if (identity.Exist(name) == elle::StatusFalse)
	escape("this user does not seem to exist");

      // destroy the identity.
      if (identity.Erase(name) == elle::StatusFalse)
	escape("unable to erase the identity file");
    }

    //
    // remove the dictionary, if necessary.
    //
    {
      lune::Dictionary	dictionary;

      // if the dictionary exists...
      if (dictionary.Exist(name) == elle::StatusTrue)
	{
	  // remove it.
	  if (dictionary.Erase(name) == elle::StatusError)
	    escape("unable to erase the dictionary");
	}
    }

    //
    // remove the phrase, if necessary.
    //
    {
      lune::Phrase	phrase;

      // if the phrase exists...
      if (phrase.Exist(name) == elle::StatusTrue)
	{
	  // remove it.
	  if (phrase.Erase(name) == elle::StatusError)
	    escape("unable to erase the phrase");
	}
    }

    //
    // remove the user directory.
    //
    {
      elle::Path	path;

      // create the user path.
      if (path.Create(lune::Lune::User) == elle::StatusError)
	escape("unable to create the path");

      // complete the path with the user name.
      if (path.Complete(elle::Piece("%USER%", name)) == elle::StatusError)
	escape("unable to complete the path");

      // clear the user directory content.
      if (elle::Directory::Clear(path) == elle::StatusError)
	escape("unable to clear the directory");

      // remove the directory.
      if (elle::Directory::Remove(path) == elle::StatusError)
	escape("unable to erase the directory");
    }

    leave();
  }

  ///
  /// this method displays information on the given user.
  ///
  elle::Status		User::Information(const elle::String&	name)
  {
    elle::String	prompt;
    elle::String	pass;
    lune::Authority	authority;
    lune::Identity	identity;
    elle::PublicKey	K;
    elle::Unique	unique;

    enter();

    // check the argument.
    if (name.empty() == true)
      escape("unable to create a user without a user name");

    // check if the user already exists.
    if (identity.Exist(name) == elle::StatusFalse)
      escape("this user does not seem to exist");

    // restore the authority's public key.
    if (K.Restore(Infinit::Authority) == elle::StatusError)
      escape("unable to restore the authority's public key");

    // create the authority based on the hard-coded public key.
    if (authority.Create(K) == elle::StatusError)
      escape("unable to create the authority");

    // prompt the user for the passphrase.
    prompt = "Enter passphrase for keypair '" + name + "': ";
    pass = elle::String(::getpass(prompt.c_str()));

    // load the identity.
    if (identity.Load(name) == elle::StatusError)
      escape("unable to load the identity");

    // verify the identity.
    if (identity.Validate(authority) != elle::StatusTrue)
      escape("the identity seems to be invalid");

    // decrypt the identity.
    if (identity.Decrypt(pass) == elle::StatusError)
      escape("unable to decrypt the identity");

    // dump the identity.
    if (identity.Dump() == elle::StatusError)
      escape("unable to dump the identity");

    // retrieve the user's public key unique.
    if (identity.pair.K.Save(unique) == elle::StatusError)
      escape("unable to save the public key's unique");

    // display the unique.
    std::cout << "[Unique] " << unique << std::endl;

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
    User::Operation	operation;
    elle::Character	option;
    elle::String	name;
    elle::String	attribute;
    elle::String	value;

    enter();

    // initialize the Elle library.
    if (elle::Elle::Initialize() == elle::StatusError)
      escape("unable to initialize Elle");

    // set up the program.
    if (elle::Program::Setup() == elle::StatusError)
      escape("unable to set up the program");

    // initialize the operation.
    operation = User::OperationUnknown;

    // allocate a new parser.
    parser = new elle::Parser(argc, argv);

    // set up the parser.
    if (parser->Register('h',
			 "help",
			 "display the help",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('c',
			 "create",
			 "create a new user",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('d',
			 "destroy",
			 "destroy an existing user",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('x',
			 "information",
			 "display information on an existing user",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('n',
			 "name",
			 "specifies a unique user name",
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
	  case 'x':
	    {
	      // check if the operation has already been set up.
	      if (operation != User::OperationUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the information operation cannot be set "
			 "concurrently to another operation");
		}

	      operation = User::OperationInformation;

	      break;
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

    // initialize the nucleus library.
    if (nucleus::Nucleus::Initialize() == elle::StatusError)
      escape("unable to initialize Nucleus");

    // initialize the Lune library.
    if (lune::Lune::Initialize() == elle::StatusError)
      escape("unable to initialize Lune");

    // initialize the Etoile.
    if (etoile::Etoile::Initialize() == elle::StatusError)
      escape("unable to initialize Etoile");

    // trigger the operation.
    switch (operation)
      {
      case User::OperationCreate:
	{
	  // create a user.
	  if (User::Create(name) == elle::StatusError)
	    escape("unable to create the user");

	  // display a message.
	  std::cout << "The user has been created successfully!"
		    << std::endl;

	  break;
	}
      case User::OperationDestroy:
	{
	  // destroy a user.
	  if (User::Destroy(name) == elle::StatusError)
	    escape("unable to destroy the user");

	  // display a message.
	  std::cout << "The user has been destroyed successfully!"
		    << std::endl;

	  break;
	}
      case User::OperationInformation:
	{
	  // display information.
	  if (User::Information(name) == elle::StatusError)
	    escape("unable to display information on the user");

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
    if (etoile::Etoile::Clean() == elle::StatusError)
      escape("unable to clean Etoile");

    // clean Lune
    if (lune::Lune::Clean() == elle::StatusError)
      escape("unable to clean Lune");

    // clean the nucleus library.
    if (nucleus::Nucleus::Clean() == elle::StatusError)
      escape("unable to clean Nucleus");

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
                             char**				argv)
{
  application::Main(argc, argv);

  expose();

  return (0);
}
