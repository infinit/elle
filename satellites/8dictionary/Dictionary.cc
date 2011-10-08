//
// ---------- header ----------------------------------------------------------
//
// project       8dictionary
//
// license       infinit
//
// author        julien quintard   [thu mar  4 17:51:46 2010]
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
    Dictionary::Operation	operation;

    enterx(instance(Infinit::Parser));

    // initialize the Elle library.
    if (elle::Elle::Initialize() == elle::StatusError)
      escape("unable to initialize Elle");

    // set up the program.
    if (elle::Program::Setup() == elle::StatusError)
      escape("unable to set up the program");

    // initialize the nucleus library.
    if (nucleus::Nucleus::Initialize() == elle::StatusError)
      escape("unable to initialize Nucleus");

    // initialize the Lune library.
    if (lune::Lune::Initialize() == elle::StatusError)
      escape("unable to initialize Lune");

    // initialize Infinit.
    if (Infinit::Initialize() == elle::StatusError)
      escape("unable to initialize Infinit");

    // initialize the Etoile library.
    if (etoile::Etoile::Initialize() == elle::StatusError)
      escape("unable to initialize Etoile");

    // initialize the operation.
    operation = Dictionary::OperationUnknown;

    // allocate a new parser.
    Infinit::Parser = new elle::Parser(argc, argv);

    // specify a program description.
    if (Infinit::Parser->Description(Infinit::Copyright) == elle::StatusError)
      escape("unable to set the description");

    // register the options.
    if (Infinit::Parser->Register(
          "Help",
	  'h',
	  "help",
	  "display the help",
	  elle::Parser::KindNone) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Profile",
	  'p',
	  "profile",
	  "specify the user name of the dictionary",
	  elle::Parser::KindRequired) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Add",
	  'a',
	  "add",
	  "add a dictionary record",
	  elle::Parser::KindNone) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Remove",
	  'r',
	  "remove",
	  "remove a dictionary record",
	  elle::Parser::KindNone) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Show",
	  's',
	  "show",
	  "show all the dictionary records",
	  elle::Parser::KindNone) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Dump",
	  'd',
	  "dump",
	  "dump a specific dictionary record",
	  elle::Parser::KindNone) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "User",
	  'u',
	  "user",
	  "indicate the type of the record to be a user",
	  elle::Parser::KindNone) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Group",
	  'g',
	  "group",
	  "indicate the type of the record to be a group",
	  elle::Parser::KindNone) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Name",
	  'n',
	  "name",
	  "specify the local UNIX/Windows/etc. name of the user/group entry",
	  elle::Parser::KindRequired) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Identifier",
	  'i',
	  "identifier",
	  "specify the user/group Infinit base64 identifier",
	  elle::Parser::KindRequired) == elle::StatusError)
      escape("unable to register the option");

    // parse.
    if (Infinit::Parser->Parse() == elle::StatusError)
      escape("unable to parse the command line");

    // test the option.
    if (Infinit::Parser->Test("Help") == elle::StatusTrue)
      {
	// display the usage.
	Infinit::Parser->Usage();

	// quit.
	leave();
      }

    // check the mutually exclusive options.
    if ((Infinit::Parser->Test("Add") == elle::StatusTrue) &&
	(Infinit::Parser->Test("Remove") == elle::StatusTrue) &&
	(Infinit::Parser->Test("Dump") == elle::StatusTrue) &&
	(Infinit::Parser->Test("Show") == elle::StatusTrue))
      {
	// display the usage.
	Infinit::Parser->Usage();

	escape("the add, remove, dump and show options are "
	       "mutually exclusive");
      }

    // check the mutually exclusive options.
    if ((Infinit::Parser->Test("User") == elle::StatusTrue) &&
	(Infinit::Parser->Test("Group") == elle::StatusTrue))
      {
	// display the usage.
	Infinit::Parser->Usage();

	escape("the user and group types cannot be activated together");
      }

    // test the option.
    if (Infinit::Parser->Test("Add") == elle::StatusTrue)
      operation = Dictionary::OperationAdd;

    // test the option.
    if (Infinit::Parser->Test("Remove") == elle::StatusTrue)
      operation = Dictionary::OperationRemove;

    // test the option.
    if (Infinit::Parser->Test("Dump") == elle::StatusTrue)
      operation = Dictionary::OperationDump;

    // test the option.
    if (Infinit::Parser->Test("Show") == elle::StatusTrue)
      operation = Dictionary::OperationShow;

    // trigger a command.
    switch (operation)
      {
      case Dictionary::OperationAdd:
	{
	  elle::String		profile;
	  Dictionary::Type	type;
	  elle::String		name;
	  elle::Unique		identifier;

	  // retrieve the profile.
	  if (Infinit::Parser->Value("Profile", profile) == elle::StatusError)
	    escape("unable to retrieve the profile value");

	  // retrieve the name.
	  if (Infinit::Parser->Value("Name", name) == elle::StatusError)
	    escape("unable to retrieve the name value");

	  // retrieve the type.
	  if (Infinit::Parser->Test("User") == elle::StatusTrue)
	    type = Dictionary::TypeUser;
	  else if (Infinit::Parser->Test("Group") == elle::StatusTrue)
	    type = Dictionary::TypeGroup;
	  else
	    escape("please specify the type of the entity: user or group");

	  // retrieve the identifier.
	  if (Infinit::Parser->Value("Identifier",
				     identifier) == elle::StatusError)
	    escape("unable to retrieve the profile value");

	  // add a record.
	  if (Dictionary::Add(profile,
			      type,
			      name,
			      identifier) == elle::StatusError)
	    escape("unable to add a mapping");

	  // display a message.
	  std::cout << "The record has been added successfully!"
		    << std::endl;

	  break;
	}
      case Dictionary::OperationRemove:
	{
	  elle::String		profile;
	  Dictionary::Type	type;
	  elle::String		name;

	  // retrieve the profile.
	  if (Infinit::Parser->Value("Profile", profile) == elle::StatusError)
	    escape("unable to retrieve the profile value");

	  // retrieve the name.
	  if (Infinit::Parser->Value("Name", name) == elle::StatusError)
	    escape("unable to retrieve the name value");

	  // retrieve the type.
	  if (Infinit::Parser->Test("User") == elle::StatusTrue)
	    type = Dictionary::TypeUser;
	  else if (Infinit::Parser->Test("Group") == elle::StatusTrue)
	    type = Dictionary::TypeGroup;
	  else
	    escape("please specify the type of the entity: user or group");

	  // remove a record.
	  if (Dictionary::Remove(profile,
				 type,
				 name) == elle::StatusError)
	    escape("unable to remove the mapping");

	  // display a message.
	  std::cout << "The record has been removed successfully!"
		    << std::endl;

	  break;
	}
      case Dictionary::OperationDump:
	{
	  elle::String		profile;
	  Dictionary::Type	type;
	  elle::String		name;

	  // retrieve the profile.
	  if (Infinit::Parser->Value("Profile", profile) == elle::StatusError)
	    escape("unable to retrieve the profile value");

	  // retrieve the name.
	  if (Infinit::Parser->Value("Name", name) == elle::StatusError)
	    escape("unable to retrieve the name value");

	  // retrieve the type.
	  if (Infinit::Parser->Test("User") == elle::StatusTrue)
	    type = Dictionary::TypeUser;
	  else if (Infinit::Parser->Test("Group") == elle::StatusTrue)
	    type = Dictionary::TypeGroup;
	  else
	    escape("please specify the type of the entity: user or group");

	  // dump the record.
	  if (Dictionary::Dump(profile,
			       type,
			       name) == elle::StatusError)
	    escape("unable to dump the mapping");

	  break;
	}
      case Dictionary::OperationShow:
	{
	  elle::String		profile;
	  Dictionary::Type	type;

	  // retrieve the profile.
	  if (Infinit::Parser->Value("Profile", profile) == elle::StatusError)
	    escape("unable to retrieve the profile value");

	  // retrieve the type.
	  if (Infinit::Parser->Test("User") == elle::StatusTrue)
	    type = Dictionary::TypeUser;
	  else if (Infinit::Parser->Test("Group") == elle::StatusTrue)
	    type = Dictionary::TypeGroup;
	  else
	    escape("please specify the type of the entity: user or group");

	  // show the records.
	  if (Dictionary::Show(profile,
			       type) == elle::StatusError)
	    escape("unable to show the mappings");

	  break;
	}
      case Dictionary::OperationUnknown:
      default:
	{
	  // display the usage.
	  Infinit::Parser->Usage();

	  escape("please specify an operation to perform");
	}
      }

    // delete the parser.
    delete Infinit::Parser;

    // waive.
    waive(Infinit::Parser);

    // clean the Etoile.
    if (etoile::Etoile::Clean() == elle::StatusError)
      escape("unable to clean Etoile");

    // clean Infinit.
    if (Infinit::Clean() == elle::StatusError)
      escape("unable to clean Infinit");

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
  try
    {
      if (application::Main(argc, argv) == elle::StatusError)
	{
	  show();

	  return (1);
	}
    }
  catch (std::exception& e)
    {
      std::cout << "The program has been terminated following "
		<< "a fatal error (" << e.what() << ")." << std::endl;

      return (1);
    }

  return (0);
}
