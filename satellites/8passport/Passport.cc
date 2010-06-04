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
// updated       julien quintard   [tue may 25 11:46:52 2010]
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
  const elle::Character		Component[] = "8passport";

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method creates a new passport for the given user in the given
  /// universe.
  ///
  elle::Status		Passport::Create(const elle::String&	user,
					 const elle::String&	universe)
  {
    lune::Authority	authority;
    lune::Identity	identity;
    lune::Memento	memento;

    enter();

    //
    // check the arguments.
    //
    {
      // check the user.
      if (user.empty() == true)
	escape("unable to create a passport without a user name");

      // check the universe.
      if (universe.empty() == true)
	escape("unable to create a passport without a universe name");
    }

    //
    // retrieve the authority.
    //
    {
      elle::String	prompt;
      elle::String	pass;

      // prompt the passport for the passphrase.
      prompt = "Enter passphrase for the authority: ";
      pass = elle::String(::getpass(prompt.c_str()));

      // load the authority.
      if (authority.Load() == elle::StatusError)
	escape("unable to load the authority");

      // decrypt the authority.
      if (authority.Decrypt(pass) == elle::StatusError)
	escape("unable to decrypt the authority");
    }

    //
    // retrieve the identity.
    //
    {
      elle::String	prompt;
      elle::String	pass;

      // prompt the passport for the passphrase.
      prompt = "Enter passphrase for keypair '" + user + "': ";
      pass = elle::String(::getpass(prompt.c_str()));

      // load the identity.
      if (identity.Load(user) == elle::StatusError)
	escape("unable to load the identity");

      // decrypt the identity.
      if (identity.Decrypt(pass) == elle::StatusError)
	escape("unable to decrypt the identity");
    }

    //
    // retrieve the memento.
    //
    {
      // load the memento.
      if (memento.Load(universe) == elle::StatusError)
	escape("unable to load the memento");
    }

    //
    // create the passport.
    //
    {
      lune::Passport	passport;

      // create a passport.
      if (passport.Create(user, universe) == elle::StatusError)
	escape("unable to create the passport");

      // encrypt the passport.
      if (passport.Encrypt(identity) == elle::StatusError)
	escape("unable to encrypt the passport");

      // seal the passport.
      if (passport.Seal(authority) == elle::StatusError)
	escape("unable to seal the passport");

      // store the passport.
      if (passport.Store(user, universe) == elle::StatusError)
	escape("unable to store the passport");
    }

    leave();
  }

  ///
  /// this method destroys an existing passport.
  ///
  elle::Status		Passport::Destroy(const elle::String&	user,
					  const elle::String&	universe)
  {
    enter();

    // XXX

    leave();
  }

  ///
  /// this method displays information on the given passport.
  ///
  elle::Status		Passport::Information(const elle::String& user,
					      const elle::String& universe)
  {
    lune::Authority	authority;
    lune::Identity	identity;
    lune::Memento	memento;

    enter();

    //
    // check the arguments.
    //
    {
      // check the user.
      if (user.empty() == true)
	escape("unable to create a passport without a user name");

      // check the universe.
      if (universe.empty() == true)
	escape("unable to create a passport without a universe name");
    }

    //
    // retrieve the authority.
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
    // retrieve the identity.
    //
    {
      elle::String	prompt;
      elle::String	pass;

      // prompt the passport for the passphrase.
      prompt = "Enter passphrase for keypair '" + user + "': ";
      pass = elle::String(::getpass(prompt.c_str()));

      // load the identity.
      if (identity.Load(user) == elle::StatusError)
	escape("unable to load the identity");

      // decrypt the identity.
      if (identity.Decrypt(pass) == elle::StatusError)
	escape("unable to decrypt the identity");
    }

    //
    // retrieve the passport.
    //
    {
      lune::Passport	passport;

      // load the passport.
      if (passport.Load(user, universe) == elle::StatusError)
	escape("unable to load the passport");

      // validate the passport.
      if (passport.Validate(authority) == elle::StatusError)
	escape("unable to validate the passport");

      // decrypt the passport.
      if (passport.Decrypt(identity) == elle::StatusError)
	escape("unable to decrypt the passport");

      // dump the passport.
      if (passport.Dump() == elle::StatusError)
	escape("unable to dump the passport");
    }

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
    Passport::Operation	operation;
    elle::Character	option;
    elle::String	universe;
    elle::String	user;

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

    // initialize the Etoile.
    if (etoile::Etoile::Initialize() == elle::StatusError)
      escape("unable to initialize Etoile");

    // initialize the operation.
    operation = Passport::OperationUnknown;

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
			 "create a new passport",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('d',
			 "destroy",
			 "destroy an existing passport",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('x',
			 "information",
			 "display information on an existing passport",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('u',
			 "user",
			 "specifies the name of the user to which the "
			 "passport is related to",
			 elle::Parser::TypeRequired) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('v',
			 "universe",
			 "specifies the name of the universe to which "
			 "the passport is related to",
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
	  case 'x':
	    {
	      // check if the operation has already been set up.
	      if (operation != Passport::OperationUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the information operation cannot be set "
			 "concurrently to another operation");
		}

	      operation = Passport::OperationInformation;

	      break;
	    }
	  case 'u':
	    {
	      // retrieve the user.
	      user.assign(optarg);

	      break;
	    }
	  case 'v':
	    {
	      // retrieve the universe.
	      universe.assign(optarg);

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
	  if (Passport::Create(user, universe) == elle::StatusError)
	    escape("unable to create the passport");

	  // display a message.
	  std::cout << "The passport has been created successfully!"
		    << std::endl;

	  break;
	}
      case Passport::OperationDestroy:
	{
	  // destroy a passport.
	  if (Passport::Destroy(user, universe) == elle::StatusError)
	    escape("unable to destroy the passport");

	  // display a message.
	  std::cout << "The passport has been destroyed successfully!"
		    << std::endl;

	  break;
	}
      case Passport::OperationInformation:
	{
	  // display information.
	  if (Passport::Information(user, universe) == elle::StatusError)
	    escape("unable to display information on the passport");

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
    if (etoile::Etoile::Clean() == elle::StatusError)
      escape("unable to clean Etoile");

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
                             char**				argv)
{
  application::Main(argc, argv);

  expose();

  return (0);
}
