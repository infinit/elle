//
// ---------- header ----------------------------------------------------------
//
// project       8access
//
// license       infinit
//
// file          /home/mycure/infinit/applications/8access/Access.cc
//
// created       julien quintard   [thu mar  4 17:51:46 2010]
// updated       julien quintard   [mon apr 19 09:38:53 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <applications/8access/Access.hh>

namespace application
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const Character	Component[] = "8access";

  ///
  /// this string contains the path to the access infinit configuration
  /// directory.
  ///
  const String		Access::Path = System::Path::Home + "/.infinit";

//
// ---------- functions -------------------------------------------------------
//

  ///
  /// this function displays the usage.
  ///
  Void			Usage()
  {
    std::cerr << "[usage] 8access -a -r -d -s -u -g "
	      << "-n {name} -k {user key} -i {group identifier}"
	      << std::endl;

    ::exit(EXIT_FAILURE);
  }

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method adds an entry.
  ///
  Status		Access::Add(const Access::Type&		type,
				    const String&		name,
				    const PublicKey&		K,
				    const
				      etoile::hole::Address&	address)
  {
    enter();

    // perform the action depending on the type.
    switch (type)
      {
      case Access::TypeUser:
	{
	  String					path =
	    Access::Path + "/access/users.map";
	  map::Map<PublicKey>				map;

	  // valid the arguments.
	  if ((name.empty() == true) || (K == PublicKey::Null))
	    Usage();

	  // load the user map.
	  if (map::Map<PublicKey>::Load(path, map) == StatusError)
	    escape("unable to load the users map");

	  // add an entry.
	  if (map.Add(name, K) == StatusError)
	    escape("unable to add the user entry to the map");

	  // store the user map.
	  if (map::Map<PublicKey>::Store(path, map) == StatusError)
	    escape("unable to store the users map");

	  break;
	}
      case Access::TypeGroup:
	{
	  String					path =
	    Access::Path + "/access/groups.map";
	  map::Map<etoile::hole::Address>		map;

	  // valid the arguments.
	  if ((name.empty() == true) ||
	      (address == etoile::hole::Address::Null))
	    Usage();

	  // load the user map.
	  if (map::Map<etoile::hole::Address>::Load(path, map) == StatusError)
	    escape("unable to load the users map");

	  // add an entry.
	  if (map.Add(name, address) == StatusError)
	    escape("unable to add the group entry to the map");

	  // store the user map.
	  if (map::Map<etoile::hole::Address>::Store(path, map) == StatusError)
	    escape("unable to store the users map");

	  break;
	}
      default:
	{
	  Usage();

	  break;
	}
      }

    leave();
  }

  ///
  /// this method removes an entry.
  ///
  Status		Access::Remove(const Access::Type&	type,
				       const String&		name)
  {
    enter();

    // valid the arguments.
    if (name.empty() == true)
      Usage();

    // perform the action depending on the type.
    switch (type)
      {
      case Access::TypeUser:
	{
	  String					path =
	    Access::Path + "/access/users.map";
	  map::Map<PublicKey>				map;

	  // load the user map.
	  if (map::Map<PublicKey>::Load(path, map) == StatusError)
	    escape("unable to load the users map");

	  // remove the entry.
	  if (map.Remove(name) == StatusError)
	    escape("unable to remove the entry");

	  // store the user map.
	  if (map::Map<PublicKey>::Store(path, map) == StatusError)
	    escape("unable to store the users map");

	  break;
	}
      case Access::TypeGroup:
	{
	  String					path =
	    Access::Path + "/access/groups.map";
	  map::Map<etoile::hole::Address>		map;

	  // load the user map.
	  if (map::Map<etoile::hole::Address>::Load(path, map) == StatusError)
	    escape("unable to load the users map");

	  // remove the entry.
	  if (map.Remove(name) == StatusError)
	    escape("unable to remove the entry");

	  // store the user map.
	  if (map::Map<etoile::hole::Address>::Store(path, map) == StatusError)
	    escape("unable to store the users map");

	  break;
	}
      default:
	{
	  Usage();

	  break;
	}
      }

    leave();
  }

  ///
  /// this method dump an entry.
  ///
  Status		Access::Dump(const Access::Type&	type,
				     const String&		name)
  {
    enter();

    // valid the arguments.
    if (name.empty() == true)
      Usage();

    // perform the action depending on the type.
    switch (type)
      {
      case Access::TypeUser:
	{
	  String					path =
	    Access::Path + "/access/users.map";
	  map::Map<PublicKey>				map;
	  const PublicKey*				K;

	  // load the user map.
	  if (map::Map<PublicKey>::Load(path, map) == StatusError)
	    escape("unable to load the users map");

	  // lookup the entry.
	  if (map.Lookup(name, K) == StatusError)
	    escape("unable to locate the entry");

	  // display the tuple.
	  std::cout << name << " " << *K << std::endl;

	  break;
	}
      case Access::TypeGroup:
	{
	  String					path =
	    Access::Path + "/access/groups.map";
	  map::Map<etoile::hole::Address>		map;
	  const ::etoile::hole::Address*		address;

	  // load the user map.
	  if (map::Map<etoile::hole::Address>::Load(path, map) == StatusError)
	    escape("unable to load the users map");

	  // lookup the entry.
	  if (map.Lookup(name, address) == StatusError)
	    escape("unable to locate the entry");

	  // display the tuple.
	  std::cout << name << " " << *address << std::endl;

	  break;
	}
      default:
	{
	  Usage();

	  break;
	}
      }

    leave();
  }

  ///
  /// this method shows all the entries.
  ///
  Status		Access::Show(const Access::Type&	type)
  {
    enter();

    // perform the action depending on the type.
    switch (type)
      {
      case Access::TypeUser:
	{
	  String					path =
	    Access::Path + "/access/users.map";
	  map::Map<PublicKey>				map;
	  map::Map<PublicKey>::Scoutor			scoutor;

	  // load the user map.
	  if (map::Map<PublicKey>::Load(path, map) == StatusError)
	    escape("unable to load the users map");

	  for (scoutor = map.container.begin();
	       scoutor != map.container.end();
	       scoutor++)
	    {
	      // show the entry.
	      std::cout << scoutor->first << " "
			<< scoutor->second << std::endl;
	    }

	  break;
	}
      case Access::TypeGroup:
	{
	  String					path =
	    Access::Path + "/access/groups.map";
	  map::Map<etoile::hole::Address>		map;
	  map::Map<etoile::hole::Address>::Scoutor	scoutor;

	  // load the user map.
	  if (map::Map<etoile::hole::Address>::Load(path, map) == StatusError)
	    escape("unable to load the users map");

	  // go through the map container.
	  for (scoutor = map.container.begin();
	       scoutor != map.container.end();
	       scoutor++)
	    {
	      // show the entry.
	      std::cout << scoutor->first << " "
			<< scoutor->second << std::endl;
	    }

	  break;
	}
      default:
	{
	  Usage();

	  break;
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
  Status		Main(Natural32				argc,
			     Character*				argv[])
  {
    Access::Operation		operation = Access::OperationUnknown;
    Access::Type		type = Access::TypeUnknown;
    String			name;
    PublicKey			K;
    etoile::hole::Address	address;
    int				opt;

    enter();

    // initialize the Elle library.
    if (Elle::Initialize() == StatusError)
      escape("unable to initialize the Elle library");

    // parse the options.
    while ((opt = ::getopt(argc, argv, "ardsugn:k:i:")) != -1)
      {
	// act according to the current option.
	switch (opt)
	  {
	  case 'a':
	    {
	      if (operation != Access::OperationUnknown)
		Usage();

	      operation = Access::OperationAdd;

	      break;
	    }
	  case 'r':
	    {
	      if (operation != Access::OperationUnknown)
		Usage();

	      operation = Access::OperationRemove;

	      break;
	    }
	  case 'd':
	    {
	      if (operation != Access::OperationUnknown)
		Usage();

	      operation = Access::OperationDump;

	      break;
	    }
	  case 's':
	    {
	      if (operation != Access::OperationUnknown)
		Usage();

	      operation = Access::OperationShow;

	      break;
	    }
	  case 'u':
	    {
	      if (type != Access::TypeUnknown)
		Usage();

	      type = Access::TypeUser;

	      break;
	    }
	  case 'g':
	    {
	      if (type != Access::TypeUnknown)
		Usage();

	      type = Access::TypeGroup;

	      break;
	    }
	  case 'n':
	    {
	      // assign the name.
	      name.assign(optarg);

	      break;
	    }
	  case 'k':
	    {
	      String		string;
	      Region		region;
	      Archive		archive;

	      // save the argument in a string.
	      string.assign(optarg);

	      // decode the base64 string.
	      if (Base64::Decode(string, region) == StatusError)
		escape("unable to decode the key argument");

	      // detach the region so that it does not get freed twice.
	      if (region.Detach() == StatusError)
		escape("unable to detach the region");

	      // prepare the archive.
	      if (archive.Prepare(region) == StatusError)
		escape("unable to prepare the archive");

	      // extract the public key.
	      if (archive.Extract(K) == StatusError)
		escape("unable to extract the public key");

	      break;
	    }
	  case 'i':
	    {
	      String		string;
	      Region		region;
	      Archive		archive;

	      // save the argument in a string.
	      string.assign(optarg);

	      // decode the base64 string.
	      if (Base64::Decode(string, region) == StatusError)
		escape("unable to decode the key argument");

	      // detach the region so that it does not get freed twice.
	      if (region.Detach() == StatusError)
		escape("unable to detach the region");

	      // prepare the archive.
	      if (archive.Prepare(region) == StatusError)
		escape("unable to prepare the archive");

	      // extract the address.
	      if (archive.Extract(address) == StatusError)
		escape("unable to extract the address");

	      break;
	    }
	  default:
	    {
	      Usage();

	      break;
	    }
	  }
      }

    // trigger a command.
    switch (operation)
      {
      case Access::OperationAdd:
	{
	  if (Access::Add(type, name, K, address) == StatusError)
	    escape("unable to add the entry");

	  break;
	}
      case Access::OperationRemove:
	{
	  if (Access::Remove(type, name) == StatusError)
	    escape("unable to remove the entry");

	  break;
	}
      case Access::OperationDump:
	{
	  if (Access::Dump(type, name) == StatusError)
	    escape("unable to dump the entry");

	  break;
	}
      case Access::OperationShow:
	{
	  if (Access::Show(type) == StatusError)
	    escape("unable to show the map");

	  break;
	}
      default:
	{
	  Usage();

	  break;
	}
      }

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
