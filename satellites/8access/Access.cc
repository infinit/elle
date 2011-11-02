//
// ---------- header ----------------------------------------------------------
//
// project       applications
//
// license       infinit
//
// author        julien.quintard   [tue nov  1 13:00:02 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <applications/8access/Access.hh>

#include <agent/Agent.hh>

namespace application
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const elle::Character		Component[] = "8access";

  ///
  /// the socket used for communicating with Etoile.
  ///
  elle::Door			Access::Door;

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method connects and authenticates to Etoile by sending the user's
  /// network-specific phrase.
  ///
  elle::Status		Access::Connect()
  {
    lune::Phrase	phrase;

    enter();

    //
    // load the phrase.
    //
    {
      // load the phrase.
      if (phrase.Load(agent::Agent::Identity.name,
		      hole::Hole::Descriptor.name) == elle::StatusError)
	escape("unable to load the phrase");
    }

    //
    // connect to the server.
    //
    {
      // create the door.
      if (Access::Door.Create() == elle::StatusError)
	escape("unable to create the door");

      // connect the door.
      if (Access::Door.Connect(
	    etoile::portal::Portal::Line,
	    elle::Channel::ModeSynchronous) == elle::StatusError)
	escape("unable to connect to the lane");
    }

    //
    // authenticate to the server.
    //
    {
      // send the user's network-specific phrase.
      if (Access::Door.Call(
	    elle::Inputs<etoile::portal::TagAuthenticate>(phrase),
	    elle::Outputs<etoile::portal::TagAuthenticated>()) ==
	  elle::StatusError)
	escape("unable to authenticate to Etoile");
    }

    leave();
  }

  ///
  /// this method looks up the access record associated with the given
  /// identifier.
  ///
  elle::Status		Access::Lookup(const etoile::path::Way&	way,
				       const nucleus::Subject&	subject)
  {
    etoile::gear::Identifier	identifier;
    nucleus::Record		record;

    enter();

    // connect to Etoile.
    if (Access::Connect() == elle::StatusError)
      goto _error;

    // invoke the lookup functionality.
    if (Access::Door.Call(
	  elle::Inputs<etoile::portal::TagAccessLookup>(identifier, subject),
	  elle::Outputs<etoile::portal::TagAccessRecord>(record)) ==
	elle::StatusError)
      goto _error;

    // XXX
    record.Dump();

  _error:
    // release the object.
    Access::Door.Send(
      elle::Inputs<etoile::portal::TagObjectDiscard>(
	identifier));

    // expose the potential errors.
    expose();

    // exit the program.
    if (elle::Program::Exit() == elle::StatusError)
      escape("unable to exit the program");

    leave();
  }

  ///
  /// this method displays all the access records for the given path.
  ///
  elle::Status		Access::Consult(const etoile::path::Way& way)
  {
    enter();

    // connect to Etoile.
    if (Access::Connect() == elle::StatusError)
      escape("unable to connect to Etoile");

    leave();
  }

  ///
  /// this method grants access to the given entity, referenced by the
  /// identifier.
  ///
  elle::Status		Access::Grant(const etoile::path::Way&	way,
				      const nucleus::Subject&	subject,
				      const nucleus::Permissions permissions)
  {
    enter();

    // XXX

    leave();
  }

  ///
  /// this method revokes an existing access.
  ///
  elle::Status		Access::Revoke(const etoile::path::Way&	way,
				       const nucleus::Subject&	subject)
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
  elle::Status		Main(elle::Natural32			argc,
			     elle::Character*			argv[])
  {
    Access::Operation	operation;

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

    // initialize the operation.
    operation = Access::OperationUnknown;

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

    // set up the hole-specific options.
    if (hole::Hole::Options() == elle::StatusError)
      escape("unable to set up the options");

    // set up the agent-specific options.
    if (agent::Agent::Options() == elle::StatusError)
      escape("unable to set up the options");

    // register the options.
    if (Infinit::Parser->Register(
          "Lookup",
	  'l',
	  "lookup",
	  "look up a specific access record",
	  elle::Parser::KindNone) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Consult",
	  'c',
	  "consult",
	  "consult the access records",
	  elle::Parser::KindNone) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Grant",
	  'g',
	  "grant",
	  "grant access to a user/group",
	  elle::Parser::KindNone) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Revoke",
	  'r',
	  "revoke",
	  "revoke an access for a user/group",
	  elle::Parser::KindNone) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Type",
	  't',
	  "type",
	  "indicate the type of the entity: user or group",
	  elle::Parser::KindRequired) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Path",
	  'p',
	  "path",
	  "indicate the path to the target object i.e file, directory or link",
	  elle::Parser::KindRequired) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Identifier",
	  'i',
	  "identifier",
	  "specify the user/group base64 identifier",
	  elle::Parser::KindRequired) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Read",
	  'R',
	  "read",
	  "indicate that the read permission must be granted",
	  elle::Parser::KindNone) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Write",
	  'W',
	  "write",
	  "indicate that the write permission must be granted",
	  elle::Parser::KindNone) == elle::StatusError)
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

    // initialize the Hole library.
    if (hole::Hole::Initialize() == elle::StatusError)
      escape("unable to initialize Hole");

    // initialize the Agent library.
    if (agent::Agent::Initialize() == elle::StatusError)
      escape("unable to initialize Agent");

    // check the mutually exclusive options.
    if ((Infinit::Parser->Test("Lookup") == elle::StatusTrue) &&
	(Infinit::Parser->Test("Consult") == elle::StatusTrue) &&
	(Infinit::Parser->Test("Grant") == elle::StatusTrue) &&
	(Infinit::Parser->Test("Revoke") == elle::StatusTrue))
      {
	// display the usage.
	Infinit::Parser->Usage();

	escape("the create, destroy and information options are "
	       "mutually exclusive");
      }

    // test the option.
    if (Infinit::Parser->Test("Lookup") == elle::StatusTrue)
      operation = Access::OperationLookup;

    // test the option.
    if (Infinit::Parser->Test("Consult") == elle::StatusTrue)
      operation = Access::OperationConsult;

    // test the option.
    if (Infinit::Parser->Test("Grant") == elle::StatusTrue)
      operation = Access::OperationGrant;

    // test the option.
    if (Infinit::Parser->Test("Revoke") == elle::StatusTrue)
      operation = Access::OperationRevoke;

    // trigger the operation.
    switch (operation)
      {
      case Access::OperationLookup:
	{
	  elle::String			path;
	  elle::String			string;
	  nucleus::Subject::Type	type;
	  nucleus::Subject		subject;

	  // retrieve the path.
	  if (Infinit::Parser->Value("Path",
				     path) == elle::StatusError)
	    escape("unable to retrieve the path value");

	  // retrieve the type.
	  if (Infinit::Parser->Value("Type",
				     string) == elle::StatusError)
	    escape("unable to retrieve the type value");

	  // convert the string into a subject type.
	  if (nucleus::Subject::Convert(string, type) == elle::StatusError)
	    escape("unable to convert the string '%s' into a "
		   "valid subject type",
		   string.c_str());

	  // build a subject depending on the type.
	  switch (type)
	    {
	    case nucleus::Subject::TypeUser:
	      {
		elle::PublicKey		K;

		// retrieve the identifier which is supposed to
		// represent a user identity i.e a public key.
		if (Infinit::Parser->Value("Identifier",
					   K) == elle::StatusError)
		  escape("unable to retrieve the user's public key "
			 "through the identifier");

		// build the subject.
		if (subject.Create(K) == elle::StatusError)
		  escape("unable to create the subject");

		break;
	      }
	    case nucleus::Subject::TypeGroup:
	      {
		// XXX
		escape("not yet supported");

		break;
	      }
	    default:
	      {
		escape("unsupported entity type '%u'",
		       type);
	      }
	    }

	  // declare additional local variables.
	  etoile::path::Way		way(path);
	  elle::Closure<
	    elle::Status,
	    elle::Parameters<
	      const etoile::path::Way&,
	      const nucleus::Subject&
	      >
	    >				closure(&Access::Lookup,
						way, subject);

	  elle::Entrance<
	    elle::Status,
	    elle::Parameters<
	      const etoile::path::Way&,
	      const nucleus::Subject&
	      >
	    >				entrance(closure);

	  // launch the program.
	  if (elle::Program::Launch() == elle::StatusError)
	    escape("an error occured while processing events");

	  break;
	}
      case Access::OperationConsult:
	{
	  /* XXX
	  elle::String		path;

	  // retrieve the path.
	  if (Infinit::Parser->Value("Path",
				     path) == elle::StatusError)
	    escape("unable to retrieve the path value");

	  // consult the access records.
	  if (Access::Consult(path) == elle::StatusError)
	    escape("unable to consult the access records");
	  */
	  break;
	}
      case Access::OperationGrant:
	{
	  /* XXX
	  elle::String		path;
	  elle::String		identifier;
	  nucleus::Permissions	permissions;

	  // retrieve the path.
	  if (Infinit::Parser->Value("Path",
				     path) == elle::StatusError)
	    escape("unable to retrieve the path value");

	  // retrieve the identifier.
	  if (Infinit::Parser->Value("Identifier",
				     identifier) == elle::StatusError)
	    escape("unable to retrieve the identifier value");

	  // initialize the permissions.
	  permissions = nucleus::PermissionNone;

	  // set the read permission, if requested.
	  if (Infinit::Parser->Test("Read") == elle::StatusTrue)
	    permissions |= nucleus::PermissionRead;

	  // set the write permission, if requested.
	  if (Infinit::Parser->Test("Write") == elle::StatusTrue)
	    permissions |= nucleus::PermissionWrite;

	  // grant the access.
	  if (Access::Grant(path,
			    identifier,
			    permissions) == elle::StatusError)
	    escape("unable to grant the access");
	  */
	  break;
	}
      case Access::OperationRevoke:
	{
	  /* XXX
	  elle::String		path;
	  elle::String		identifier;

	  // retrieve the path.
	  if (Infinit::Parser->Value("Path",
				     path) == elle::StatusError)
	    escape("unable to retrieve the path value");

	  // retrieve the identifier.
	  if (Infinit::Parser->Value("Identifier",
				     identifier) == elle::StatusError)
	    escape("unable to retrieve the identifier value");

	  // revoke the access.
	  if (Access::Revoke(path, identifier) == elle::StatusError)
	    escape("unable to revoke the access");
	  */
	  break;
	}
      case Access::OperationUnknown:
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

    // clean Hole.
    if (hole::Hole::Clean() == elle::StatusError)
      escape("unable to clean Hole");

    // clean the Agent library.
    if (agent::Agent::Clean() == elle::StatusError)
      escape("unable to clean Agent");

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
