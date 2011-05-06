//
// ---------- header ----------------------------------------------------------
//
// project       8dictionary
//
// license       infinit
//
// file          /home/mycure/infinit/applications/8dictionary/Dictionary.cc
//
// created       julien quintard   [thu mar  4 17:51:46 2010]
// updated       julien quintard   [fri may  6 14:26:02 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <applications/8dictionary/Dictionary.hh>

namespace application
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const elle::Character		Component[] = "8dictionary";

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method adds an entry.
  ///
  elle::Status		Dictionary::Add(const elle::String&	profile,
					const Dictionary::Type&	type,
					const elle::String&	name,
					const elle::Unique&	identifier)
  {
    lune::Dictionary	dictionary;

    enter();

    //
    // check the arguments.
    //
    {
      lune::Identity	identity;

      // check the name.
      if (name.empty() == true)
	escape("please specify a name");

      // check the identifier.
      if (identifier.empty() == true)
	escape("please specify an identifier");

      // does the user identity exist.
      if (identity.Exist(profile) == elle::StatusFalse)
	escape("this user does not seem to exist");
    }

    //
    // retrieve the dictionary.
    //
    {
      // load the dictionary if it exists.
      if (dictionary.Exist(profile) == elle::StatusTrue)
	{
	  // load the dictionary.
	  if (dictionary.Load(profile) == elle::StatusError)
	    escape("unable to load the dictionary");
	}
    }

    // perform the action depending on the type.
    switch (type)
      {
      case Dictionary::TypeUser:
	{
	  elle::PublicKey	K;

	  // restore the public key from the identifier.
	  if (K.Restore(identifier) == elle::StatusError)
	    escape("unable to restore the public key");

	  // add an entry.
	  if (dictionary.users.Add(name, K) == elle::StatusError)
	    escape("unable to add the user entry to the dictionary");

	  break;
	}
      case Dictionary::TypeGroup:
	{
	  nucleus::Address	address;

	  // restore the address from the identifier.
	  if (address.Restore(identifier) == elle::StatusError)
	    escape("unable to restore the address");

	  // add an entry.
	  if (dictionary.groups.Add(name, address) == elle::StatusError)
	    escape("unable to add the group entry to the dictionary");

	  break;
	}
      case Dictionary::TypeUnknown:
      default:
	{
	  escape("please specify a type of entry to add");
	}
      }

    //
    // store the dictionary.
    //
    {
      // store the dictionary file.
      if (dictionary.Store(profile) == elle::StatusError)
	escape("unable to store the dictionary");
    }

    leave();
  }

  ///
  /// this method removes an entry.
  ///
  elle::Status		Dictionary::Remove(const elle::String&	profile,
					   const Dictionary::Type& type,
					   const elle::String&	name)
  {
    lune::Dictionary	dictionary;

    enter();

    //
    // check the arguments.
    //
    {
      lune::Identity	identity;

      // check the name.
      if (name.empty() == true)
	escape("please specify a name");

      // does the user identity exist.
      if (identity.Exist(profile) == elle::StatusFalse)
	escape("this user does not seem to exist");
    }

    //
    // retrieve the dictionary.
    //
    {
      // load the dictionary if it exists.
      if (dictionary.Exist(profile) == elle::StatusTrue)
	{
	  // load the dictionary file.
	  if (dictionary.Load(profile) == elle::StatusError)
	    escape("unable to load the dictionary");
	}
    }

    // perform the action depending on the type.
    switch (type)
      {
      case Dictionary::TypeUser:
	{
	  // remove an entry.
	  if (dictionary.users.Remove(name) == elle::StatusError)
	    escape("unable to remove the user entry");

	  break;
	}
      case Dictionary::TypeGroup:
	{
	  // remove an entry.
	  if (dictionary.groups.Remove(name) == elle::StatusError)
	    escape("unable to remove the group entry");

	  break;
	}
      case Dictionary::TypeUnknown:
      default:
	{
	  escape("please specify a type of entry to remove");
	}
      }

    //
    // store the dictionary back.
    //
    {
      // store the dictionary file.
      if (dictionary.Store(profile) == elle::StatusError)
	escape("unable to store the dictionary");
    }

    leave();
  }

  ///
  /// this method shows all the entries.
  ///
  elle::Status		Dictionary::Show(const elle::String&	profile,
					 const Dictionary::Type& type)
  {
    lune::Dictionary	dictionary;

    enter();

    //
    // check the arguments.
    //
    {
      lune::Identity	identity;

      // does the user identity exist.
      if (identity.Exist(profile) == elle::StatusFalse)
	escape("this user does not seem to exist");
    }

    //
    // retrieve the dictionary.
    //
    {
      // load the dictionary if it exists.
      if (dictionary.Exist(profile) == elle::StatusTrue)
	{
	  // load the dictionary file.
	  if (dictionary.Load(profile) == elle::StatusError)
	    escape("unable to load the dictionary");
	}
    }

    // perform the action depending on the type.
    switch (type)
      {
      case Dictionary::TypeUser:
	{
	  lune::Map<elle::PublicKey>::Scoutor scoutor;

	  // go through the user dictionary.
	  for (scoutor = dictionary.users.container.begin();
	       scoutor != dictionary.users.container.end();
	       scoutor++)
	    {
	      lune::Map<elle::PublicKey>::Entry* entry = *scoutor;

	      std::cout << entry->name << " :: "
			<< entry->value << std::endl;
	    }

	  break;
	}
      case Dictionary::TypeGroup:
	{
	  lune::Map<nucleus::Address>::Scoutor scoutor;

	  // go through the group dictionary.
	  for (scoutor = dictionary.groups.container.begin();
	       scoutor != dictionary.groups.container.end();
	       scoutor++)
	    {
	      lune::Map<nucleus::Address>::Entry* entry = *scoutor;

	      std::cout << entry->name << " :: "
			<< entry->value << std::endl;
	    }

	  break;
	}
      case Dictionary::TypeUnknown:
      default:
	{
	  escape("please specify a type of entry to show");
	}
      }

    leave();
  }

  ///
  /// this method dump an entry.
  ///
  elle::Status		Dictionary::Dump(const elle::String&	profile,
					 const Dictionary::Type& type,
					 const elle::String&	name)
  {
    lune::Dictionary	dictionary;

    enter();

    //
    // check the arguments.
    //
    {
      lune::Identity	identity;

      // check the name.
      if (name.empty() == true)
	escape("please specify a name");

      // does the user identity exist.
      if (identity.Exist(profile) == elle::StatusFalse)
	escape("this user does not seem to exist");
    }

    //
    // retrieve the dictionary.
    //
    {
      // load the dictionary if it exists.
      if (dictionary.Exist(profile) == elle::StatusTrue)
	{
	  // load the dictionary file.
	  if (dictionary.Load(profile) == elle::StatusError)
	    escape("unable to load the dictionary");
	}
    }

    // perform the action depending on the type.
    switch (type)
      {
      case Dictionary::TypeUser:
	{
	  elle::PublicKey*	K;
	  elle::Unique		unique;

	  // retrieve the entry.
	  if (dictionary.users.Lookup(name, K) != elle::StatusTrue)
	    escape("unable to look up the user entry");

	  // retrive the public key's unique.
	  if (K->Save(unique) == elle::StatusError)
	    escape("unable to save the public key's unique");

	  // dump the information.
	  std::cout << name << " :: "
		    << unique << std::endl;

	  break;
	}
      case Dictionary::TypeGroup:
	{
	  nucleus::Address*	address;
	  elle::Unique		unique;

	  // retrieve the entry.
	  if (dictionary.groups.Lookup(name, address) != elle::StatusTrue)
	    escape("unable to look up the group entry");

	  // retrive the address's unique.
	  if (address->Save(unique) == elle::StatusError)
	    escape("unable to save the address's unique");

	  // dump the information.
	  std::cout << name << " :: "
		    << unique << std::endl;

	  break;
	}
      case Dictionary::TypeUnknown:
      default:
	{
	  escape("please specify a type of entry to dump");
	}
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
    elle::Parser*		parser;
    Dictionary::Operation	operation;
    Dictionary::Type		type;
    elle::String		profile;
    elle::Character		option;
    elle::String		name;
    elle::Unique		identifier;

    enter();

    // initialize the Elle library.
    if (elle::Elle::Initialize() == elle::StatusError)
      escape("unable to initialize Elle");

    // set up the program.
    if (elle::Program::Setup() == elle::StatusError)
      escape("unable to set up the program");

    // initialize the operation.
    operation = Dictionary::OperationUnknown;

    // initialize the type.
    type = Dictionary::TypeUnknown;

    // allocate a new parser.
    parser = new elle::Parser(argc, argv);

    // set up the parser.
    if (parser->Register('h',
			 "help",
			 "display the help",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('p',
			 "profile",
			 "specify the user name of the dictionary",
			 elle::Parser::TypeRequired) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('a',
			 "add",
			 "add an dictionary record",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('r',
			 "remove",
			 "remove an dictionary record",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('s',
			 "show",
			 "show all the dictionary records",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('d',
			 "dump",
			 "dump a specific dictionary record",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('u',
			 "user",
			 "indicate the type of the record to be a user",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('g',
			 "group",
			 "indicate the type of the record to be a group",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('n',
			 "name",
			 "specifies the local UNIX/Windows/etc. name of "
			 "the user/group entry",
			 elle::Parser::TypeRequired) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('i',
			 "identifier",
			 "specifies the user/group Infinit base64 identifier",
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
	  case 'p':
	    {
	      // retrieve the user profile.
	      profile.assign(optarg);

	      break;
	    }
	  case 'a':
	    {
	      // check if the operation has already been set up.
	      if (operation != Dictionary::OperationUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the add operation cannot be set concurrently "
			 "to another operation");
		}

	      operation = Dictionary::OperationAdd;

	      break;
	    }
	  case 'r':
	    {
	      // check if the operation has already been set up.
	      if (operation != Dictionary::OperationUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the remove operation cannot be set concurrently to "
			 "another operation");
		}

	      operation = Dictionary::OperationRemove;

	      break;
	    }
	  case 's':
	    {
	      // check if the operation has already been set up.
	      if (operation != Dictionary::OperationUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the dump operation cannot be set "
			 "concurrently to another operation");
		}

	      operation = Dictionary::OperationShow;

	      break;
	    }
	  case 'd':
	    {
	      // check if the operation has already been set up.
	      if (operation != Dictionary::OperationUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the show operation cannot be set "
			 "concurrently to another operation");
		}

	      operation = Dictionary::OperationDump;

	      break;
	    }
	  case 'u':
	    {
	      // check if the type has already been set up.
	      if (type != Dictionary::TypeUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the user type cannot be set "
			 "concurrently to another type");
		}

	      // set the type.
	      type = Dictionary::TypeUser;

	      break;
	    }
	  case 'g':
	    {
	      // check if the type has already been set up.
	      if (type != Dictionary::TypeUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the group type cannot be set "
			 "concurrently to another type");
		}

	      // set the type.
	      type = Dictionary::TypeGroup;

	      break;
	    }
	  case 'n':
	    {
	      // retrieve the user/group name.
	      name.assign(optarg);

	      break;
	    }
	  case 'i':
	    {
	      // retrieve the user/group identifier.
	      identifier.assign(optarg);

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

    // trigger a command.
    switch (operation)
      {
      case Dictionary::OperationAdd:
	{
	  // add a record.
	  if (Dictionary::Add(profile,
			      type,
			      name,
			      identifier) == elle::StatusError)
	    escape("unable to add a mapping");

	  // display a message.
	  std::cout << "The mapping has been added successfully!"
		    << std::endl;

	  break;
	}
      case Dictionary::OperationRemove:
	{
	  // remove a record.
	  if (Dictionary::Remove(profile,
				 type,
				 name) == elle::StatusError)
	    escape("unable to remove the mapping");

	  // display a message.
	  std::cout << "The mapping has been removed successfully!"
		    << std::endl;

	  break;
	}
      case Dictionary::OperationShow:
	{
	  // show the records.
	  if (Dictionary::Show(profile,
			       type) == elle::StatusError)
	    escape("unable to show the mappings");

	  break;
	}
      case Dictionary::OperationDump:
	{
	  // dump the record.
	  if (Dictionary::Dump(profile,
			       type,
			       name) == elle::StatusError)
	    escape("unable to dump the mapping");

	  break;
	}
      case Dictionary::OperationUnknown:
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
