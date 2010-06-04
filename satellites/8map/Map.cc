//
// ---------- header ----------------------------------------------------------
//
// project       8map
//
// license       infinit
//
// file          /home/mycure/infinit/applications/8map/Map.cc
//
// created       julien quintard   [thu mar  4 17:51:46 2010]
// updated       julien quintard   [fri may 28 15:19:59 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <applications/8map/Map.hh>

namespace application
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const elle::Character		Component[] = "8map";

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method adds an entry.
  ///
  elle::Status		Map::Add(const Map::Type&		type,
				 const elle::String&		name,
				 const elle::Unique&		identifier)
  {
    lune::Associat	associat;

    enter();

    //
    // check the arguments.
    //
    {
      // check the name.
      if (name.empty() == true)
	escape("please specify a name");

      // check the identifier.
      if (identifier.empty() == true)
	escape("please specify an identifier");
    }

    //
    // retrieve the associat.
    //
    {
      // load the associat file.
      if (associat.Load() == elle::StatusError)
	escape("unable to load the associat");
    }

    // perform the action depending on the type.
    switch (type)
      {
      case Map::TypeUser:
	{
	  elle::PublicKey	K;

	  // restore the public key from the identifier.
	  if (K.Restore(identifier) == elle::StatusError)
	    escape("unable to restore the public key");

	  // add an entry.
	  if (associat.users.Add(name, K) == elle::StatusError)
	    escape("unable to add the user entry to the map");

	  break;
	}
      case Map::TypeGroup:
	{
	  etoile::hole::Address	address;

	  // restore the address from the identifier.
	  if (address.Restore(identifier) == elle::StatusError)
	    escape("unable to restore the address");

	  // add an entry.
	  if (associat.groups.Add(name, address) == elle::StatusError)
	    escape("unable to add the group entry to the map");

	  break;
	}
      case Map::TypeUnknown:
      default:
	{
	  escape("please specify a type of entry to add");
	}
      }

    //
    // store the associat back.
    //
    {
      // store the associat file.
      if (associat.Store() == elle::StatusError)
	escape("unable to store the associat");
    }

    leave();
  }

  ///
  /// this method removes an entry.
  ///
  elle::Status		Map::Remove(const Map::Type&		type,
				    const elle::String&		name)
  {
    lune::Associat	associat;

    enter();

    //
    // check the arguments.
    //
    {
      // check the name.
      if (name.empty() == true)
	escape("please specify a name");
    }

    //
    // retrieve the associat.
    //
    {
      // load the associat file.
      if (associat.Load() == elle::StatusError)
	escape("unable to load the associat");
    }

    // perform the action depending on the type.
    switch (type)
      {
      case Map::TypeUser:
	{
	  // remove an entry.
	  if (associat.users.Remove(name) == elle::StatusError)
	    escape("unable to remove the user entry");

	  break;
	}
      case Map::TypeGroup:
	{
	  // remove an entry.
	  if (associat.groups.Remove(name) == elle::StatusError)
	    escape("unable to remove the group entry");

	  break;
	}
      case Map::TypeUnknown:
      default:
	{
	  escape("please specify a type of entry to remove");
	}
      }

    //
    // store the associat back.
    //
    {
      // store the associat file.
      if (associat.Store() == elle::StatusError)
	escape("unable to store the associat");
    }

    leave();
  }

  ///
  /// this method dump an entry.
  ///
  elle::Status		Map::Dump(const Map::Type&		type,
				  const elle::String&		name)
  {
    lune::Associat	associat;

    enter();

    //
    // check the arguments.
    //
    {
      // check the name.
      if (name.empty() == true)
	escape("please specify a name");
    }

    //
    // retrieve the associat.
    //
    {
      // load the associat file.
      if (associat.Load() == elle::StatusError)
	escape("unable to load the associat");
    }

    // perform the action depending on the type.
    switch (type)
      {
      case Map::TypeUser:
	{
	  elle::PublicKey*		K;
	  elle::Unique			unique;

	  // retrieve the entry.
	  if (associat.users.Lookup(name, K) != elle::StatusTrue)
	    escape("unable to look up the user entry");

	  // retrive the public key's unique.
	  if (K->Save(unique) == elle::StatusError)
	    escape("unable to save the public key's unique");

	  // dump the information.
	  std::cout << name << " :: "
		    << unique << std::endl;

	  break;
	}
      case Map::TypeGroup:
	{
	  etoile::hole::Address*	address;
	  elle::Unique			unique;

	  // retrieve the entry.
	  if (associat.groups.Lookup(name, address) != elle::StatusTrue)
	    escape("unable to look up the group entry");

	  // retrive the address's unique.
	  if (address->Save(unique) == elle::StatusError)
	    escape("unable to save the address's unique");

	  // dump the information.
	  std::cout << name << " :: "
		    << unique << std::endl;

	  break;
	}
      case Map::TypeUnknown:
      default:
	{
	  escape("please specify a type of entry to dump");
	}
      }

    leave();
  }

  ///
  /// this method shows all the entries.
  ///
  elle::Status		Map::Show(const Map::Type&		type)
  {
    lune::Associat	associat;

    enter();

    //
    // retrieve the associat.
    //
    {
      // load the associat file.
      if (associat.Load() == elle::StatusError)
	escape("unable to load the associat");
    }

    // perform the action depending on the type.
    switch (type)
      {
      case Map::TypeUser:
	{
	  lune::Map<elle::PublicKey>::Scoutor	scoutor;

	  // go through the user map.
	  for (scoutor = associat.users.container.begin();
	       scoutor != associat.users.container.end();
	       scoutor++)
	    {
	      lune::Map<elle::PublicKey>::Entry* entry = *scoutor;

	      std::cout << entry->name << " :: "
			<< entry->value << std::endl;
	    }

	  break;
	}
      case Map::TypeGroup:
	{
	  lune::Map<etoile::hole::Address>::Scoutor	scoutor;

	  // go through the group map.
	  for (scoutor = associat.groups.container.begin();
	       scoutor != associat.groups.container.end();
	       scoutor++)
	    {
	      lune::Map<etoile::hole::Address>::Entry* entry = *scoutor;

	      std::cout << entry->name << " :: "
			<< entry->value << std::endl;
	    }

	  break;
	}
      case Map::TypeUnknown:
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
    elle::Parser*	parser;
    Map::Operation	operation;
    Map::Type		type;
    elle::Character	option;
    elle::String	name;
    elle::Unique	identifier;

    enter();

    // initialize the Elle library.
    if (elle::Elle::Initialize() == elle::StatusError)
      escape("unable to initialize Elle");

    // set up the program.
    if (elle::Program::Setup() == elle::StatusError)
      escape("unable to set up the program");

    // initialize the operation.
    operation = Map::OperationUnknown;

    // initialize the type.
    type = Map::TypeUnknown;

    // allocate a new parser.
    parser = new elle::Parser(argc, argv);

    // set up the parser.
    if (parser->Register('h',
			 "help",
			 "display the help",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('a',
			 "add",
			 "add an map record",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('r',
			 "remove",
			 "remove an map record",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('d',
			 "dump",
			 "dump all the map records",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('s',
			 "show",
			 "show a specific map record",
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

    if (parser->Register('d',
			 "dump",
			 "dump a specific map record",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('s',
			 "show",
			 "show all the map records",
			 elle::Parser::TypeNone) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('n',
			 "name",
			 "specifies the name of the user/group entry",
			 elle::Parser::TypeRequired) == elle::StatusError)
      escape("unable to register the option");

    if (parser->Register('i',
			 "identifier",
			 "specifies the user/group Infinit identifier",
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
	  case 'a':
	    {
	      // check if the operation has already been set up.
	      if (operation != Map::OperationUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the add operation cannot be set concurrently "
			 "to another operation");
		}

	      operation = Map::OperationAdd;

	      break;
	    }
	  case 'r':
	    {
	      // check if the operation has already been set up.
	      if (operation != Map::OperationUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the remove operation cannot be set concurrently to "
			 "another operation");
		}

	      operation = Map::OperationRemove;

	      break;
	    }
	  case 'd':
	    {
	      // check if the operation has already been set up.
	      if (operation != Map::OperationUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the dump operation cannot be set "
			 "concurrently to another operation");
		}

	      operation = Map::OperationDump;

	      break;
	    }
	  case 's':
	    {
	      // check if the operation has already been set up.
	      if (operation != Map::OperationUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the show operation cannot be set "
			 "concurrently to another operation");
		}

	      operation = Map::OperationShow;

	      break;
	    }
	  case 'u':
	    {
	      // check if the type has already been set up.
	      if (type != Map::TypeUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the user type cannot be set "
			 "concurrently to another type");
		}

	      // set the type.
	      type = Map::TypeUser;

	      break;
	    }
	  case 'g':
	    {
	      // check if the type has already been set up.
	      if (type != Map::TypeUnknown)
		{
		  // display the usage.
		  parser->Usage();

		  escape("the group type cannot be set "
			 "concurrently to another type");
		}

	      // set the type.
	      type = Map::TypeGroup;

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

    // initialize the Lune library.
    if (lune::Lune::Initialize(lune::Lune::ModeUser) == elle::StatusError)
      escape("unable to initialize Lune");

    // set up the lune environment.
    if (lune::Lune::Setup(
    // initialize the Etoile.
    if (etoile::Etoile::Initialize() == elle::StatusError)
      escape("unable to initialize Etoile");

    // trigger a command.
    switch (operation)
      {
      case Map::OperationAdd:
	{
	  // add a record.
	  if (Map::Add(type, name, identifier) == elle::StatusError)
	    escape("unable to add a mapping");

	  // display a message.
	  std::cout << "The mapping has been added successfully!"
		    << std::endl;

	  break;
	}
      case Map::OperationRemove:
	{
	  // remove a record.
	  if (Map::Remove(type, name) == elle::StatusError)
	    escape("unable to remove the mapping");

	  // display a message.
	  std::cout << "The mapping has been removed successfully!"
		    << std::endl;

	  break;
	}
      case Map::OperationDump:
	{
	  // dump the record.
	  if (Map::Dump(type, name) == elle::StatusError)
	    escape("unable to dump the mapping");

	  break;
	}
      case Map::OperationShow:
	{
	  // show the records.
	  if (Map::Show(type) == elle::StatusError)
	    escape("unable to show the mappings");

	  break;
	}
      case Map::OperationUnknown:
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
