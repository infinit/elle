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
    // report the problem.
    report("the socket has been unexpectedly disconnected");

    // expose the report.
    expose();

    // exit.
    elle::concurrency::Program::Exit();

    return elle::Status::Ok;
  }

  ///
  /// this callback is triggered whenever an error occurs on the socket.
  ///
  elle::Status          Access::Error(const elle::String&       error)
  {
    // report the given error.
    report(error.c_str());

    // expose the report.
    expose();

    // exit.
    elle::concurrency::Program::Exit();

    return elle::Status::Ok;
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

    //
    // load the phrase.
    //
    {
      // load the phrase.
      if (phrase.Load(Infinit::Network) == elle::Status::Error)
        escape("unable to load the phrase");
    }

    //
    // connect to the server.
    //
    {
      // create the socket.
      if (Access::Socket.Create() == elle::Status::Error)
        escape("unable to create the socket");

      // subscribe to the signal.
      if (Access::Socket.signal.disconnected.Subscribe(
            elle::concurrency::Callback<>::Infer(
              &Access::Disconnected)) == elle::Status::Error)
        escape("unable to subscribe to the signal");

      // subscribe to the signal.
      if (Access::Socket.signal.error.Subscribe(
            elle::concurrency::Callback<>::Infer(
              &Access::Error)) == elle::Status::Error)
        escape("unable to subscribe to the signal");

      // connect the socket.
      if (Access::Socket.Connect(
            phrase.portal,
            elle::Socket::ModeSynchronous) == elle::Status::Error)
        escape("unable to connect to the lane");
    }

    //
    // authenticate to the server.
    //
    {
      // send the user's network-specific phrase.
      if (Access::Socket.Call(
            elle::network::Inputs<etoile::portal::TagAuthenticate>(phrase.pass),
            elle::network::Outputs<etoile::portal::TagAuthenticated>()) ==
          elle::Status::Error)
        escape("unable to authenticate to Etoile");
    }

    return elle::Status::Ok;
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

    // connect to Etoile.
    if (Access::Connect() == elle::Status::Error)
      goto _error;

    // resolve the path.
    if (Access::Socket.Call(
          elle::network::Inputs<etoile::portal::TagPathResolve>(way),
          elle::network::Outputs<etoile::portal::TagPathChemin>(chemin)) ==
        elle::Status::Error)
      goto _error;

    // load the object.
    if (Access::Socket.Call(
          elle::network::Inputs<etoile::portal::TagObjectLoad>(chemin),
          elle::network::Outputs<etoile::portal::TagIdentifier>(identifier)) ==
        elle::Status::Error)
      goto _error;

    // lookup the access record.
    if (Access::Socket.Call(
          elle::network::Inputs<etoile::portal::TagAccessLookup>(identifier, subject),
          elle::network::Outputs<etoile::portal::TagAccessRecord>(record)) ==
        elle::Status::Error)
      goto _error;

    // discard the object.
    if (Access::Socket.Call(
          elle::network::Inputs<etoile::portal::TagObjectDiscard>(identifier),
          elle::network::Outputs<elle::TagOk>()) == elle::Status::Error)
      goto _error;

    // dump the record.
    record.Dump();

  _error:
    // release the object.
    Access::Socket.Send(
      elle::network::Inputs<etoile::portal::TagObjectDiscard>(
        identifier));

    // expose the potential errors.
    expose();

    // exit the program.
    elle::concurrency::Program::Exit();

    return elle::Status::Ok;
  }

  ///
  /// this method displays all the access records for the given path.
  ///
  elle::Status          Access::Consult(const etoile::path::Way& way)
  {
    etoile::path::Chemin                chemin;
    etoile::gear::Identifier            identifier;
    nucleus::Range<nucleus::Record>     range;

    // connect to Etoile.
    if (Access::Connect() == elle::Status::Error)
      goto _error;

    // resolve the path.
    if (Access::Socket.Call(
          elle::network::Inputs<etoile::portal::TagPathResolve>(way),
          elle::network::Outputs<etoile::portal::TagPathChemin>(chemin)) ==
        elle::Status::Error)
      goto _error;

    // load the object.
    if (Access::Socket.Call(
          elle::network::Inputs<etoile::portal::TagObjectLoad>(chemin),
          elle::network::Outputs<etoile::portal::TagIdentifier>(identifier)) ==
        elle::Status::Error)
      goto _error;

    // consult the object access.
    if (Access::Socket.Call(
          elle::network::Inputs<etoile::portal::TagAccessConsult>(
            identifier,
            elle::Type<nucleus::Index>::Minimum,
            elle::Type<nucleus::Size>::Maximum),
          elle::network::Outputs<etoile::portal::TagAccessRange>(range)) ==
        elle::Status::Error)
      goto _error;

    // discard the object.
    if (Access::Socket.Call(
          elle::network::Inputs<etoile::portal::TagObjectDiscard>(identifier),
          elle::network::Outputs<elle::TagOk>()) == elle::Status::Error)
      goto _error;

    // dump the range.
    range.Dump();

  _error:
    // release the object.
    Access::Socket.Send(
      elle::network::Inputs<etoile::portal::TagObjectDiscard>(
        identifier));

    // expose the potential errors.
    expose();

    // exit the program.
    elle::concurrency::Program::Exit();

    return elle::Status::Ok;
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

    // connect to Etoile.
    if (Access::Connect() == elle::Status::Error)
      goto _error;

    // locate the path i.e is the way located in the Infinit file system.
    if (Access::Socket.Call(
          elle::network::Inputs<etoile::portal::TagPathLocate>(way),
          elle::network::Outputs<etoile::portal::TagPathWay>(path)) ==
        elle::Status::Error)
      goto _error;

    // XXX
    printf("HERE\n");
    path.Dump();
    exit(0);

    // resolve the path.
    if (Access::Socket.Call(
          elle::network::Inputs<etoile::portal::TagPathResolve>(path),
          elle::network::Outputs<etoile::portal::TagPathChemin>(chemin)) ==
        elle::Status::Error)
      goto _error;

    // load the object.
    if (Access::Socket.Call(
          elle::network::Inputs<etoile::portal::TagObjectLoad>(chemin),
          elle::network::Outputs<etoile::portal::TagIdentifier>(identifier)) ==
        elle::Status::Error)
      goto _error;

    // lookup the access record.
    if (Access::Socket.Call(
          elle::network::Inputs<etoile::portal::TagAccessGrant>(identifier,
                                                       subject,
                                                       permissions),
          elle::network::Outputs<elle::TagOk>()) == elle::Status::Error)
      goto _error;

    // store the object.
    if (Access::Socket.Call(
          elle::network::Inputs<etoile::portal::TagObjectStore>(identifier),
          elle::network::Outputs<elle::TagOk>()) == elle::Status::Error)
      goto _error;

  _error:
    // release the object.
    Access::Socket.Send(
      elle::network::Inputs<etoile::portal::TagObjectDiscard>(
        identifier));

    // expose the potential errors.
    expose();

    // exit the program.
    elle::concurrency::Program::Exit();

    return elle::Status::Ok;
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

    // connect to Etoile.
    if (Access::Connect() == elle::Status::Error)
      goto _error;

    // resolve the path.
    if (Access::Socket.Call(
          elle::network::Inputs<etoile::portal::TagPathResolve>(way),
          elle::network::Outputs<etoile::portal::TagPathChemin>(chemin)) ==
        elle::Status::Error)
      goto _error;

    // load the object.
    if (Access::Socket.Call(
          elle::network::Inputs<etoile::portal::TagObjectLoad>(chemin),
          elle::network::Outputs<etoile::portal::TagIdentifier>(identifier)) ==
        elle::Status::Error)
      goto _error;

    // revoke the access for the given subject.
    if (Access::Socket.Call(
          elle::network::Inputs<etoile::portal::TagAccessRevoke>(identifier, subject),
          elle::network::Outputs<elle::TagOk>()) == elle::Status::Error)
      goto _error;

    // store the object.
    if (Access::Socket.Call(
          elle::network::Inputs<etoile::portal::TagObjectStore>(identifier),
          elle::network::Outputs<elle::TagOk>()) == elle::Status::Error)
      goto _error;

  _error:
    // release the object.
    Access::Socket.Send(
      elle::network::Inputs<etoile::portal::TagObjectDiscard>(
        identifier));

    // expose the potential errors.
    expose();

    // exit the program.
    elle::concurrency::Program::Exit();

    return elle::Status::Ok;
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
    if (elle::Elle::Initialize() == elle::Status::Error)
      escape("unable to initialize Elle");

    // set up the program.
    if (elle::concurrency::Program::Setup() == elle::Status::Error)
      escape("unable to set up the program");

    // initialize the nucleus library.
    if (nucleus::Nucleus::Initialize() == elle::Status::Error)
      escape("unable to initialize Nucleus");

    // initialize the Lune library.
    if (lune::Lune::Initialize() == elle::Status::Error)
      escape("unable to initialize Lune");

    // initialize Infinit.
    if (Infinit::Initialize() == elle::Status::Error)
      escape("unable to initialize Infinit");

    // initialize the operation.
    operation = Access::OperationUnknown;

    // allocate a new parser.
    Infinit::Parser = new elle::Parser(argc, argv);

    // specify a program description.
    if (Infinit::Parser->Description(Infinit::Copyright) == elle::Status::Error)
      escape("unable to set the description");

    // register the options.
    if (Infinit::Parser->Register(
          "Help",
          'h',
          "help",
          "display the help",
          elle::Parser::KindNone) == elle::Status::Error)
      escape("unable to register the option");

    // register the option.
    if (Infinit::Parser->Register(
          "User",
          'u',
          "user",
          "specifies the name of the user",
          elle::Parser::KindRequired) == elle::Status::Error)
      escape("unable to register the option");

    // register the option.
    if (Infinit::Parser->Register(
          "Network",
          'n',
          "network",
          "specifies the name of the network",
          elle::Parser::KindRequired) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Lookup",
          'l',
          "lookup",
          "look up a specific access record",
          elle::Parser::KindNone) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Consult",
          'c',
          "consult",
          "consult the access records",
          elle::Parser::KindNone) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Grant",
          'g',
          "grant",
          "grant access to a user/group",
          elle::Parser::KindNone) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Revoke",
          'r',
          "revoke",
          "revoke an access for a user/group",
          elle::Parser::KindNone) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Type",
          't',
          "type",
          "indicate the type of the entity: user or group",
          elle::Parser::KindRequired) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Path",
          'p',
          "path",
          "indicate the local absolute path to the target object "
          "i.e file, directory or link",
          elle::Parser::KindRequired) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Identifier",
          'i',
          "identifier",
          "specify the user/group base64 identifier",
          elle::Parser::KindRequired) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Read",
          'R',
          "read",
          "indicate that the read permission must be granted",
          elle::Parser::KindNone) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Write",
          'W',
          "write",
          "indicate that the write permission must be granted",
          elle::Parser::KindNone) == elle::Status::Error)
      escape("unable to register the option");

    // parse.
    if (Infinit::Parser->Parse() == elle::Status::Error)
      escape("unable to parse the command line");

    // test the option.
    if (Infinit::Parser->Test("Help") == elle::Status::True)
      {
        // display the usage.
        Infinit::Parser->Usage();

        // quit.
        return elle::Status::Ok;
      }

    // retrieve the user name.
    if (Infinit::Parser->Value("User",
                               Infinit::User) == elle::Status::Error)
      {
        // display the usage.
        Infinit::Parser->Usage();

        escape("unable to retrieve the user name");
      }

    // retrieve the network name.
    if (Infinit::Parser->Value("Network",
                               Infinit::Network) == elle::Status::Error)
      {
        // display the usage.
        Infinit::Parser->Usage();

        escape("unable to retrieve the network name");
      }

    // initialize the Hole library.
    if (hole::Hole::Initialize() == elle::Status::Error)
      escape("unable to initialize Hole");

    // initialize the Agent library.
    if (agent::Agent::Initialize() == elle::Status::Error)
      escape("unable to initialize Agent");

    // check the mutually exclusive options.
    if ((Infinit::Parser->Test("Lookup") == elle::Status::True) &&
        (Infinit::Parser->Test("Consult") == elle::Status::True) &&
        (Infinit::Parser->Test("Grant") == elle::Status::True) &&
        (Infinit::Parser->Test("Revoke") == elle::Status::True))
      {
        // display the usage.
        Infinit::Parser->Usage();

        escape("the create, destroy and information options are "
               "mutually exclusive");
      }

    // test the option.
    if (Infinit::Parser->Test("Lookup") == elle::Status::True)
      operation = Access::OperationLookup;

    // test the option.
    if (Infinit::Parser->Test("Consult") == elle::Status::True)
      operation = Access::OperationConsult;

    // test the option.
    if (Infinit::Parser->Test("Grant") == elle::Status::True)
      operation = Access::OperationGrant;

    // test the option.
    if (Infinit::Parser->Test("Revoke") == elle::Status::True)
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
                                     path) == elle::Status::Error)
            escape("unable to retrieve the path value");

          // retrieve the type.
          if (Infinit::Parser->Value("Type",
                                     string) == elle::Status::Error)
            escape("unable to retrieve the type value");

          // convert the string into a subject type.
          if (nucleus::Subject::Convert(string, type) == elle::Status::Error)
            escape("unable to convert the string '%s' into a "
                   "valid subject type",
                   string.c_str());

          // build a subject depending on the type.
          switch (type)
            {
            case nucleus::Subject::TypeUser:
              {
                elle::cryptography::PublicKey         K;

                // retrieve the identifier which is supposed to
                // represent a user identity i.e a public key.
                if (Infinit::Parser->Value("Identifier",
                                           K) == elle::Status::Error)
                  escape("unable to retrieve the user's public key "
                         "through the identifier");

                // build the subject.
                if (subject.Create(K) == elle::Status::Error)
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
            elle::radix::Parameters<
              const etoile::path::Way&,
              const nucleus::Subject&
              >
            >                           closure(&Access::Lookup,
                                                way, subject);
          elle::Entrance<
            elle::Status,
            elle::radix::Parameters<
              const etoile::path::Way&,
              const nucleus::Subject&
              >
            >                           entrance(closure);

          // launch the program.
          if (elle::concurrency::Program::Launch() == elle::Status::Error)
            escape("an error occured while processing events");

          break;
        }
      case Access::OperationConsult:
        {
          elle::String                  path;

          // retrieve the path.
          if (Infinit::Parser->Value("Path",
                                     path) == elle::Status::Error)
            escape("unable to retrieve the path value");

          // declare additional local variables.
          etoile::path::Way             way(path);
          elle::Closure<
            elle::Status,
            elle::radix::Parameters<
              const etoile::path::Way&
              >
            >                           closure(&Access::Consult,
                                                way);
          elle::Entrance<
            elle::Status,
            elle::radix::Parameters<
              const etoile::path::Way&
              >
            >                           entrance(closure);

          // launch the program.
          if (elle::concurrency::Program::Launch() == elle::Status::Error)
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
                                     path) == elle::Status::Error)
            escape("unable to retrieve the path value");

          // retrieve the type.
          if (Infinit::Parser->Value("Type",
                                     string) == elle::Status::Error)
            escape("unable to retrieve the type value");

          // convert the string into a subject type.
          if (nucleus::Subject::Convert(string, type) == elle::Status::Error)
            escape("unable to convert the string '%s' into a "
                   "valid subject type",
                   string.c_str());

          // build a subject depending on the type.
          switch (type)
            {
            case nucleus::Subject::TypeUser:
              {
                elle::cryptography::PublicKey         K;

                // retrieve the identifier which is supposed to
                // represent a user identity i.e a public key.
                if (Infinit::Parser->Value("Identifier",
                                           K) == elle::Status::Error)
                  escape("unable to retrieve the user's public key "
                         "through the identifier");

                // build the subject.
                if (subject.Create(K) == elle::Status::Error)
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
          if (Infinit::Parser->Test("Read") == elle::Status::True)
            permissions |= nucleus::PermissionRead;

          // grant the write permission, if requested.
          if (Infinit::Parser->Test("Write") == elle::Status::True)
            permissions |= nucleus::PermissionWrite;

          // declare additional local variables.
          etoile::path::Way             way(path);
          elle::Closure<
            elle::Status,
            elle::radix::Parameters<
              const etoile::path::Way&,
              const nucleus::Subject&,
              const nucleus::Permissions
              >
            >                           closure(&Access::Grant,
                                                way, subject, permissions);
          elle::Entrance<
            elle::Status,
            elle::radix::Parameters<
              const etoile::path::Way&,
              const nucleus::Subject&,
              const nucleus::Permissions
              >
            >                           entrance(closure);

          // launch the program.
          if (elle::concurrency::Program::Launch() == elle::Status::Error)
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
                                     path) == elle::Status::Error)
            escape("unable to retrieve the path value");

          // retrieve the type.
          if (Infinit::Parser->Value("Type",
                                     string) == elle::Status::Error)
            escape("unable to retrieve the type value");

          // convert the string into a subject type.
          if (nucleus::Subject::Convert(string, type) == elle::Status::Error)
            escape("unable to convert the string '%s' into a "
                   "valid subject type",
                   string.c_str());

          // build a subject depending on the type.
          switch (type)
            {
            case nucleus::Subject::TypeUser:
              {
                elle::cryptography::PublicKey         K;

                // retrieve the identifier which is supposed to
                // represent a user identity i.e a public key.
                if (Infinit::Parser->Value("Identifier",
                                           K) == elle::Status::Error)
                  escape("unable to retrieve the user's public key "
                         "through the identifier");

                // build the subject.
                if (subject.Create(K) == elle::Status::Error)
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
            elle::radix::Parameters<
              const etoile::path::Way&,
              const nucleus::Subject&
              >
            >                           closure(&Access::Revoke,
                                                way, subject);
          elle::Entrance<
            elle::Status,
            elle::radix::Parameters<
              const etoile::path::Way&,
              const nucleus::Subject&
              >
            >                           entrance(closure);

          // launch the program.
          if (elle::concurrency::Program::Launch() == elle::Status::Error)
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
    if (hole::Hole::Clean() == elle::Status::Error)
      escape("unable to clean Hole");

    // clean the Agent library.
    if (agent::Agent::Clean() == elle::Status::Error)
      escape("unable to clean Agent");

    // clean Infinit.
    if (Infinit::Clean() == elle::Status::Error)
      escape("unable to clean Infinit");

    // clean Lune
    if (lune::Lune::Clean() == elle::Status::Error)
      escape("unable to clean Lune");

    // clean the nucleus library.
    if (nucleus::Nucleus::Clean() == elle::Status::Error)
      escape("unable to clean Nucleus");

    // clean Elle.
    if (elle::Elle::Clean() == elle::Status::Error)
      escape("unable to clean Elle");

    return elle::Status::Ok;
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
      if (satellite::Main(argc, argv) == elle::Status::Error)
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
