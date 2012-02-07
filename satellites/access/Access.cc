//
// ---------- header ----------------------------------------------------------
//
// project       access
//
// license       infinit
//
// author        julien quintard   [tue nov  1 13:00:02 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <satellites/access/Access.hh>

#include <agent/Agent.hh>

namespace satellite
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const elle::Character         Component[] = "8access";

  ///
  /// the socket used for communicating with Etoile.
  ///
  elle::LocalSocket                     Access::Socket;

//
// ---------- static callbacks ------------------------------------------------
//

  ///
  /// this callback is triggered whenver the socket gets disconnected.
  ///
  elle::Status          Access::Disconnected()
  {
    ;

    // report the problem.
    report("the socket has been unexpectedly disconnected");

    // expose the report.
    expose();

    // exit.
    elle::Program::Exit();

    return elle::StatusOk;
  }

  ///
  /// this callback is triggered whenever an error occurs on the socket.
  ///
  elle::Status          Access::Error(const elle::String&       error)
  {
    ;

    // report the given error.
    report(error.c_str());

    // expose the report.
    expose();

    // exit.
    elle::Program::Exit();

    return elle::StatusOk;
  }

//
// ---------- static methods --------------------------------------------------
//

  ///
  /// this method connects and authenticates to Etoile by sending the user's
  /// network-specific phrase.
  ///
  elle::Status          Access::Connect()
  {
    lune::Phrase        phrase;

    ;

    //
    // load the phrase.
    //
    {
      // load the phrase.
      if (phrase.Load(Infinit::Network) == elle::StatusError)
        escape("unable to load the phrase");
    }

    //
    // connect to the server.
    //
    {
      // create the socket.
      if (Access::Socket.Create() == elle::StatusError)
        escape("unable to create the socket");

      // subscribe to the signal.
      if (Access::Socket.signal.disconnected.Subscribe(
            elle::Callback<>::Infer(
              &Access::Disconnected)) == elle::StatusError)
        escape("unable to subscribe to the signal");

      // subscribe to the signal.
      if (Access::Socket.signal.error.Subscribe(
            elle::Callback<>::Infer(
              &Access::Error)) == elle::StatusError)
        escape("unable to subscribe to the signal");

      // connect the socket.
      if (Access::Socket.Connect(
            phrase.portal,
            elle::Socket::ModeSynchronous) == elle::StatusError)
        escape("unable to connect to the lane");
    }

    //
    // authenticate to the server.
    //
    {
      // send the user's network-specific phrase.
      if (Access::Socket.Call(
            elle::Inputs<etoile::portal::TagAuthenticate>(phrase.pass),
            elle::Outputs<etoile::portal::TagAuthenticated>()) ==
          elle::StatusError)
        escape("unable to authenticate to Etoile");
    }

    return elle::StatusOk;
  }

  ///
  /// this method looks up the access record associated with the given
  /// identifier.
  ///
  elle::Status          Access::Lookup(const etoile::path::Way& way,
                                       const nucleus::Subject&  subject)
  {
    etoile::path::Chemin        chemin;
    etoile::gear::Identifier    identifier;
    nucleus::Record             record;

    ;

    // connect to Etoile.
    if (Access::Connect() == elle::StatusError)
      goto _error;

    // resolve the path.
    if (Access::Socket.Call(
          elle::Inputs<etoile::portal::TagPathResolve>(way),
          elle::Outputs<etoile::portal::TagPathChemin>(chemin)) ==
        elle::StatusError)
      goto _error;

    // load the object.
    if (Access::Socket.Call(
          elle::Inputs<etoile::portal::TagObjectLoad>(chemin),
          elle::Outputs<etoile::portal::TagIdentifier>(identifier)) ==
        elle::StatusError)
      goto _error;

    // lookup the access record.
    if (Access::Socket.Call(
          elle::Inputs<etoile::portal::TagAccessLookup>(identifier, subject),
          elle::Outputs<etoile::portal::TagAccessRecord>(record)) ==
        elle::StatusError)
      goto _error;

    // discard the object.
    if (Access::Socket.Call(
          elle::Inputs<etoile::portal::TagObjectDiscard>(identifier),
          elle::Outputs<elle::TagOk>()) == elle::StatusError)
      goto _error;

    // dump the record.
    record.Dump();

  _error:
    // release the object.
    Access::Socket.Send(
      elle::Inputs<etoile::portal::TagObjectDiscard>(
        identifier));

    // expose the potential errors.
    expose();

    // exit the program.
    elle::Program::Exit();

    return elle::StatusOk;
  }

  ///
  /// this method displays all the access records for the given path.
  ///
  elle::Status          Access::Consult(const etoile::path::Way& way)
  {
    etoile::path::Chemin                chemin;
    etoile::gear::Identifier            identifier;
    nucleus::Range<nucleus::Record>     range;

    ;

    // connect to Etoile.
    if (Access::Connect() == elle::StatusError)
      goto _error;

    // resolve the path.
    if (Access::Socket.Call(
          elle::Inputs<etoile::portal::TagPathResolve>(way),
          elle::Outputs<etoile::portal::TagPathChemin>(chemin)) ==
        elle::StatusError)
      goto _error;

    // load the object.
    if (Access::Socket.Call(
          elle::Inputs<etoile::portal::TagObjectLoad>(chemin),
          elle::Outputs<etoile::portal::TagIdentifier>(identifier)) ==
        elle::StatusError)
      goto _error;

    // consult the object access.
    if (Access::Socket.Call(
          elle::Inputs<etoile::portal::TagAccessConsult>(
            identifier,
            elle::Type<nucleus::Index>::Minimum,
            elle::Type<nucleus::Size>::Maximum),
          elle::Outputs<etoile::portal::TagAccessRange>(range)) ==
        elle::StatusError)
      goto _error;

    // discard the object.
    if (Access::Socket.Call(
          elle::Inputs<etoile::portal::TagObjectDiscard>(identifier),
          elle::Outputs<elle::TagOk>()) == elle::StatusError)
      goto _error;

    // dump the range.
    range.Dump();

  _error:
    // release the object.
    Access::Socket.Send(
      elle::Inputs<etoile::portal::TagObjectDiscard>(
        identifier));

    // expose the potential errors.
    expose();

    // exit the program.
    elle::Program::Exit();

    return elle::StatusOk;
  }

  ///
  /// this method grants access to the given entity, referenced by the
  /// identifier.
  ///
  elle::Status          Access::Grant(const etoile::path::Way&  way,
                                      const nucleus::Subject&   subject,
                                      const nucleus::Permissions permissions)
  {
    etoile::path::Chemin        chemin;
    etoile::gear::Identifier    identifier;
    etoile::path::Way           path;

    ;

    // connect to Etoile.
    if (Access::Connect() == elle::StatusError)
      goto _error;

    // locate the path i.e is the way located in the Infinit file system.
    if (Access::Socket.Call(
          elle::Inputs<etoile::portal::TagPathLocate>(way),
          elle::Outputs<etoile::portal::TagPathWay>(path)) ==
        elle::StatusError)
      goto _error;

    // XXX
    printf("HERE\n");
    path.Dump();
    exit(0);

    // resolve the path.
    if (Access::Socket.Call(
          elle::Inputs<etoile::portal::TagPathResolve>(path),
          elle::Outputs<etoile::portal::TagPathChemin>(chemin)) ==
        elle::StatusError)
      goto _error;

    // load the object.
    if (Access::Socket.Call(
          elle::Inputs<etoile::portal::TagObjectLoad>(chemin),
          elle::Outputs<etoile::portal::TagIdentifier>(identifier)) ==
        elle::StatusError)
      goto _error;

    // lookup the access record.
    if (Access::Socket.Call(
          elle::Inputs<etoile::portal::TagAccessGrant>(identifier,
                                                       subject,
                                                       permissions),
          elle::Outputs<elle::TagOk>()) == elle::StatusError)
      goto _error;

    // store the object.
    if (Access::Socket.Call(
          elle::Inputs<etoile::portal::TagObjectStore>(identifier),
          elle::Outputs<elle::TagOk>()) == elle::StatusError)
      goto _error;

  _error:
    // release the object.
    Access::Socket.Send(
      elle::Inputs<etoile::portal::TagObjectDiscard>(
        identifier));

    // expose the potential errors.
    expose();

    // exit the program.
    elle::Program::Exit();

    return elle::StatusOk;
  }

  ///
  /// this method revokes an existing access.
  ///
  elle::Status          Access::Revoke(const etoile::path::Way& way,
                                       const nucleus::Subject&  subject)
  {
    etoile::path::Chemin        chemin;
    etoile::gear::Identifier    identifier;
    nucleus::Record             record;

    ;

    // connect to Etoile.
    if (Access::Connect() == elle::StatusError)
      goto _error;

    // resolve the path.
    if (Access::Socket.Call(
          elle::Inputs<etoile::portal::TagPathResolve>(way),
          elle::Outputs<etoile::portal::TagPathChemin>(chemin)) ==
        elle::StatusError)
      goto _error;

    // load the object.
    if (Access::Socket.Call(
          elle::Inputs<etoile::portal::TagObjectLoad>(chemin),
          elle::Outputs<etoile::portal::TagIdentifier>(identifier)) ==
        elle::StatusError)
      goto _error;

    // revoke the access for the given subject.
    if (Access::Socket.Call(
          elle::Inputs<etoile::portal::TagAccessRevoke>(identifier, subject),
          elle::Outputs<elle::TagOk>()) == elle::StatusError)
      goto _error;

    // store the object.
    if (Access::Socket.Call(
          elle::Inputs<etoile::portal::TagObjectStore>(identifier),
          elle::Outputs<elle::TagOk>()) == elle::StatusError)
      goto _error;

  _error:
    // release the object.
    Access::Socket.Send(
      elle::Inputs<etoile::portal::TagObjectDiscard>(
        identifier));

    // expose the potential errors.
    expose();

    // exit the program.
    elle::Program::Exit();

    return elle::StatusOk;
  }

//
// ---------- functions -------------------------------------------------------
//

  ///
  /// the main function.
  ///
  elle::Status          Main(elle::Natural32                    argc,
                             elle::Character*                   argv[])
  {
    Access::Operation   operation;

    // XXX Infinit::Parser is not deleted in case of errors

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

    // register the option.
    if (Infinit::Parser->Register(
          "User",
          'u',
          "user",
          "specifies the name of the user",
          elle::Parser::KindRequired) == elle::StatusError)
      escape("unable to register the option");

    // register the option.
    if (Infinit::Parser->Register(
          "Network",
          'n',
          "network",
          "specifies the name of the network",
          elle::Parser::KindRequired) == elle::StatusError)
      escape("unable to register the option");

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
          "indicate the local absolute path to the target object "
          "i.e file, directory or link",
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
        return elle::StatusOk;
      }

    // retrieve the user name.
    if (Infinit::Parser->Value("User",
                               Infinit::User) == elle::StatusError)
      {
        // display the usage.
        Infinit::Parser->Usage();

        escape("unable to retrieve the user name");
      }

    // retrieve the network name.
    if (Infinit::Parser->Value("Network",
                               Infinit::Network) == elle::StatusError)
      {
        // display the usage.
        Infinit::Parser->Usage();

        escape("unable to retrieve the network name");
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
          elle::String                  path;
          elle::String                  string;
          nucleus::Subject::Type        type;
          nucleus::Subject              subject;

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
                elle::PublicKey         K;

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
          etoile::path::Way             way(path);
          elle::Closure<
            elle::Status,
            elle::Parameters<
              const etoile::path::Way&,
              const nucleus::Subject&
              >
            >                           closure(&Access::Lookup,
                                                way, subject);
          elle::Entrance<
            elle::Status,
            elle::Parameters<
              const etoile::path::Way&,
              const nucleus::Subject&
              >
            >                           entrance(closure);

          // launch the program.
          if (elle::Program::Launch() == elle::StatusError)
            escape("an error occured while processing events");

          break;
        }
      case Access::OperationConsult:
        {
          elle::String                  path;

          // retrieve the path.
          if (Infinit::Parser->Value("Path",
                                     path) == elle::StatusError)
            escape("unable to retrieve the path value");

          // declare additional local variables.
          etoile::path::Way             way(path);
          elle::Closure<
            elle::Status,
            elle::Parameters<
              const etoile::path::Way&
              >
            >                           closure(&Access::Consult,
                                                way);
          elle::Entrance<
            elle::Status,
            elle::Parameters<
              const etoile::path::Way&
              >
            >                           entrance(closure);

          // launch the program.
          if (elle::Program::Launch() == elle::StatusError)
            escape("an error occured while processing events");

          break;
        }
      case Access::OperationGrant:
        {
          elle::String                  path;
          elle::String                  string;
          nucleus::Subject::Type        type;
          nucleus::Subject              subject;
          nucleus::Permissions          permissions;

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
                elle::PublicKey         K;

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

          // initialize the permissions to none.
          permissions = nucleus::PermissionNone;

          // grant the read permission, if requested.
          if (Infinit::Parser->Test("Read") == elle::StatusTrue)
            permissions |= nucleus::PermissionRead;

          // grant the write permission, if requested.
          if (Infinit::Parser->Test("Write") == elle::StatusTrue)
            permissions |= nucleus::PermissionWrite;

          // declare additional local variables.
          etoile::path::Way             way(path);
          elle::Closure<
            elle::Status,
            elle::Parameters<
              const etoile::path::Way&,
              const nucleus::Subject&,
              const nucleus::Permissions
              >
            >                           closure(&Access::Grant,
                                                way, subject, permissions);
          elle::Entrance<
            elle::Status,
            elle::Parameters<
              const etoile::path::Way&,
              const nucleus::Subject&,
              const nucleus::Permissions
              >
            >                           entrance(closure);

          // launch the program.
          if (elle::Program::Launch() == elle::StatusError)
            escape("an error occured while processing events");

          break;
        }
      case Access::OperationRevoke:
        {
          elle::String                  path;
          elle::String                  string;
          nucleus::Subject::Type        type;
          nucleus::Subject              subject;

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
                elle::PublicKey         K;

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
          etoile::path::Way             way(path);
          elle::Closure<
            elle::Status,
            elle::Parameters<
              const etoile::path::Way&,
              const nucleus::Subject&
              >
            >                           closure(&Access::Revoke,
                                                way, subject);
          elle::Entrance<
            elle::Status,
            elle::Parameters<
              const etoile::path::Way&,
              const nucleus::Subject&
              >
            >                           entrance(closure);

          // launch the program.
          if (elle::Program::Launch() == elle::StatusError)
            escape("an error occured while processing events");

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
    Infinit::Parser = nullptr;

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

    return elle::StatusOk;
  }

}

//
// ---------- main ------------------------------------------------------------
//

///
/// this is the program entry point.
///
int                     main(int                                argc,
                             char**                             argv)
{
  try
    {
      if (satellite::Main(argc, argv) == elle::StatusError)
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
