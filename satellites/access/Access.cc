#include <satellites/access/Access.hh>

#include <elle/Elle.hh>
#include <elle/network/TCPSocket.hh>
#include <elle/network/Inputs.hh>
#include <elle/network/Outputs.hh>
#include <elle/utility/Parser.hh>
#include <elle/standalone/Report.hh>
#include <elle/concurrency/Program.hh>

#include <etoile/gear/Identifier.hh>
#include <etoile/path/Way.hh>
#include <etoile/path/Chemin.hh>
#include <etoile/portal/Manifest.hh>

#include <nucleus/Nucleus.hh>
#include <nucleus/neutron/Range.hh>
#include <nucleus/neutron/Record.hh>
#include <nucleus/neutron/Subject.hh>

#include <lune/Lune.hh>
#include <lune/Phrase.hh>

#include <elle/idiom/Close.hh>
# include <boost/foreach.hpp>
# include <limits>
#include <elle/idiom/Open.hh>

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
  elle::network::TCPSocket* Access::socket = nullptr;

//
// ---------- static methods --------------------------------------------------
//

  elle::Status
  Access::Display(nucleus::neutron::Record const& record)
  {
    switch (record.subject.type)
      {
      case nucleus::neutron::Subject::TypeUser:
        {
          elle::io::Unique unique;

          // convert the public key into a human-kind-of-readable string.
          if (record.subject.user->Save(unique) == elle::Status::Error)
            escape("unable to save the public key's unique");

          std::cout << "User"
                    << " "
                    << unique
                    << " "
                    << std::dec << static_cast<elle::Natural32>(record.permissions) << std::endl;

          break;
        }
      case nucleus::neutron::Subject::TypeGroup:
        {
          elle::io::Unique unique;

          // convert the group's address into a human-kind-of-readable string.
          if (record.subject.group->Save(unique) == elle::Status::Error)
            escape("unable to save the address' unique");

          std::cout << "Group"
                    << " "
                    << unique
                    << " "
                    << std::dec << static_cast<elle::Natural32>(record.permissions) << std::endl;

          break;
        }
      default:
        {
          break;
        }
      }

    return (elle::Status::Ok);
  }

  ///
  /// this method connects and authenticates to Etoile by sending the user's
  /// network-specific phrase.
  ///
  elle::Status          Access::Connect()
  {
    lune::Phrase        phrase;

    // load the phrase.
    if (phrase.Load(Infinit::Network) == elle::Status::Error)
      escape("unable to load the phrase");

    // connect to the server.
    reactor::network::TCPSocket* socket =
      new reactor::network::TCPSocket(elle::concurrency::scheduler(),
                                      elle::String("127.0.0.1"),
                                      phrase.port);

    Access::socket = new elle::network::TCPSocket(socket, true);

    // authenticate.
    if (Access::socket->Call(
          elle::network::Inputs<etoile::portal::TagAuthenticate>(phrase.pass),
          elle::network::Outputs<etoile::portal::TagAuthenticated>()) ==
        elle::Status::Error)
      escape("unable to authenticate to Etoile");

    return elle::Status::Ok;
  }

  ///
  /// this method looks up the access record associated with the given
  /// identifier.
  ///
  elle::Status          Access::Lookup(const etoile::path::Way& way,
                                       const nucleus::neutron::Subject&  subject)
  {
    etoile::path::Chemin        chemin;
    etoile::gear::Identifier    identifier;
    nucleus::neutron::Record             record;

    // connect to Etoile.
    if (Access::Connect() == elle::Status::Error)
      goto _error;

    // resolve the path.
    if (Access::socket->Call(
          elle::network::Inputs<etoile::portal::TagPathResolve>(way),
          elle::network::Outputs<etoile::portal::TagPathChemin>(chemin)) ==
        elle::Status::Error)
      goto _error;

    // load the object.
    if (Access::socket->Call(
          elle::network::Inputs<etoile::portal::TagObjectLoad>(chemin),
          elle::network::Outputs<etoile::portal::TagIdentifier>(identifier)) ==
        elle::Status::Error)
      goto _error;

    // lookup the access record.
    if (Access::socket->Call(
          elle::network::Inputs<etoile::portal::TagAccessLookup>(identifier, subject),
          elle::network::Outputs<etoile::portal::TagAccessRecord>(record)) ==
        elle::Status::Error)
      goto _error;

    // discard the object.
    if (Access::socket->Call(
          elle::network::Inputs<etoile::portal::TagObjectDiscard>(identifier),
          elle::network::Outputs<elle::TagOk>()) == elle::Status::Error)
      goto _error;

    // display the record.
    if (Access::Display(record) == elle::Status::Error)
      goto _error;

    return elle::Status::Ok;

  _error:
    // release the object.
    Access::socket->send(
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
    nucleus::neutron::Range<nucleus::neutron::Record>     range;

    // connect to Etoile.
    if (Access::Connect() == elle::Status::Error)
      goto _error;

    // resolve the path.
    if (Access::socket->Call(
          elle::network::Inputs<etoile::portal::TagPathResolve>(way),
          elle::network::Outputs<etoile::portal::TagPathChemin>(chemin)) ==
        elle::Status::Error)
      goto _error;

    // load the object.
    if (Access::socket->Call(
          elle::network::Inputs<etoile::portal::TagObjectLoad>(chemin),
          elle::network::Outputs<etoile::portal::TagIdentifier>(identifier)) ==
        elle::Status::Error)
      goto _error;

    // consult the object access.
    if (Access::socket->Call(
          elle::network::Inputs<etoile::portal::TagAccessConsult>(
            identifier,
            std::numeric_limits<nucleus::neutron::Index>::min(),
            std::numeric_limits<nucleus::neutron::Size>::max()),
          elle::network::Outputs<etoile::portal::TagAccessRange>(range)) ==
        elle::Status::Error)
      goto _error;

    // discard the object.
    if (Access::socket->Call(
          elle::network::Inputs<etoile::portal::TagObjectDiscard>(identifier),
          elle::network::Outputs<elle::TagOk>()) == elle::Status::Error)
      goto _error;

    // dump the range.
    BOOST_FOREACH(auto record, range.container)
      {
        // display the record.
        if (Access::Display(*record) == elle::Status::Error)
          goto _error;
      }

    return elle::Status::Ok;

  _error:
    // release the object.
    Access::socket->send(
      elle::network::Inputs<etoile::portal::TagObjectDiscard>(
        identifier));

    // expose the potential errors.
    expose();

    return elle::Status::Ok;
  }

  ///
  /// this method grants access to the given entity, referenced by the
  /// identifier.
  ///
  elle::Status          Access::Grant(const etoile::path::Way&  way,
                                      const nucleus::neutron::Subject&   subject,
                                      const nucleus::neutron::Permissions permissions)
  {
    etoile::path::Chemin        chemin;
    etoile::gear::Identifier    identifier;

    // connect to Etoile.
    if (Access::Connect() == elle::Status::Error)
      goto _error;

    // resolve the path.
    if (Access::socket->Call(
          elle::network::Inputs<etoile::portal::TagPathResolve>(way),
          elle::network::Outputs<etoile::portal::TagPathChemin>(chemin)) ==
        elle::Status::Error)
      goto _error;

    // load the object.
    if (Access::socket->Call(
          elle::network::Inputs<etoile::portal::TagObjectLoad>(chemin),
          elle::network::Outputs<etoile::portal::TagIdentifier>(identifier)) ==
        elle::Status::Error)
      goto _error;

    // lookup the access record.
    if (Access::socket->Call(
          elle::network::Inputs<etoile::portal::TagAccessGrant>(identifier,
                                                       subject,
                                                       permissions),
          elle::network::Outputs<elle::TagOk>()) == elle::Status::Error)
      goto _error;

    // store the object.
    if (Access::socket->Call(
          elle::network::Inputs<etoile::portal::TagObjectStore>(identifier),
          elle::network::Outputs<elle::TagOk>()) == elle::Status::Error)
      goto _error;

    return elle::Status::Ok;

  _error:
    // release the object.
    Access::socket->send(
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
                                       const nucleus::neutron::Subject&  subject)
  {
    etoile::path::Chemin        chemin;
    etoile::gear::Identifier    identifier;
    nucleus::neutron::Record             record;

    // connect to Etoile.
    if (Access::Connect() == elle::Status::Error)
      goto _error;

    // resolve the path.
    if (Access::socket->Call(
          elle::network::Inputs<etoile::portal::TagPathResolve>(way),
          elle::network::Outputs<etoile::portal::TagPathChemin>(chemin)) ==
        elle::Status::Error)
      goto _error;

    // load the object.
    if (Access::socket->Call(
          elle::network::Inputs<etoile::portal::TagObjectLoad>(chemin),
          elle::network::Outputs<etoile::portal::TagIdentifier>(identifier)) ==
        elle::Status::Error)
      goto _error;

    // revoke the access for the given subject.
    if (Access::socket->Call(
          elle::network::Inputs<etoile::portal::TagAccessRevoke>(identifier, subject),
          elle::network::Outputs<elle::TagOk>()) == elle::Status::Error)
      goto _error;

    // store the object.
    if (Access::socket->Call(
          elle::network::Inputs<etoile::portal::TagObjectStore>(identifier),
          elle::network::Outputs<elle::TagOk>()) == elle::Status::Error)
      goto _error;

    return elle::Status::Ok;

  _error:
    // release the object.
    Access::socket->send(
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
    Infinit::Parser = new elle::utility::Parser(argc, argv);

    // specify a program description.
    if (Infinit::Parser->Description(Infinit::Copyright) == elle::Status::Error)
      escape("unable to set the description");

    // register the options.
    if (Infinit::Parser->Register(
          "Help",
          'h',
          "help",
          "display the help",
          elle::utility::Parser::KindNone) == elle::Status::Error)
      escape("unable to register the option");

    // register the option.
    if (Infinit::Parser->Register(
          "User",
          'u',
          "user",
          "specifies the name of the user",
          elle::utility::Parser::KindRequired) == elle::Status::Error)
      escape("unable to register the option");

    // register the option.
    if (Infinit::Parser->Register(
          "Network",
          'n',
          "network",
          "specifies the name of the network",
          elle::utility::Parser::KindRequired) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Lookup",
          'l',
          "lookup",
          "look up a specific access record",
          elle::utility::Parser::KindNone) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Consult",
          'c',
          "consult",
          "consult the access records",
          elle::utility::Parser::KindNone) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Grant",
          'g',
          "grant",
          "grant access to a user/group",
          elle::utility::Parser::KindNone) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Revoke",
          'r',
          "revoke",
          "revoke an access for a user/group",
          elle::utility::Parser::KindNone) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Type",
          't',
          "type",
          "indicate the type of the entity: user or group",
          elle::utility::Parser::KindRequired) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Path",
          'p',
          "path",
          "indicate the local absolute path to the target object "
          "i.e file, directory or link",
          elle::utility::Parser::KindRequired) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Identifier",
          'i',
          "identifier",
          "specify the user/group base64 identifier",
          elle::utility::Parser::KindRequired) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Read",
          'R',
          "read",
          "indicate that the read permission must be granted",
          elle::utility::Parser::KindNone) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Write",
          'W',
          "write",
          "indicate that the write permission must be granted",
          elle::utility::Parser::KindNone) == elle::Status::Error)
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
          nucleus::neutron::Subject::Type        type;
          nucleus::neutron::Subject              subject;

          // retrieve the path.
          if (Infinit::Parser->Value("Path",
                                     path) == elle::Status::Error)
            escape("unable to retrieve the path value");

          // retrieve the type.
          if (Infinit::Parser->Value("Type",
                                     string) == elle::Status::Error)
            escape("unable to retrieve the type value");

          // convert the string into a subject type.
          if (nucleus::neutron::Subject::Convert(string, type) == elle::Status::Error)
            escape("unable to convert the string '%s' into a "
                   "valid subject type",
                   string.c_str());

          // build a subject depending on the type.
          switch (type)
            {
            case nucleus::neutron::Subject::TypeUser:
              {
                elle::cryptography::PublicKey         K;
                std::string                           res;

                // retrieve the identifier which is supposed to
                // represent a user identity i.e a public key.
                if (Infinit::Parser->Value(
                      "Identifier",
                      res) == elle::Status::Error)
                  escape("unable to retrieve the identifier");

                if (K.Restore(res) == elle::Status::Error)
                  escape("unable to retrieve the user's public key "
                         "through the identifier");

                // build the subject.
                if (subject.Create(K) == elle::Status::Error)
                  escape("unable to create the subject");

                break;
              }
            case nucleus::neutron::Subject::TypeGroup:
              {
                // XXX
                escape("not yet supported");

                break;
              }
            default:
              {
                escape("unsupported entity type '%u'", type);
              }
            }

          // declare additional local variables.
          etoile::path::Way             way(path);

          if (Access::Lookup(way, subject) == elle::Status::Error)
            escape("unable to lookup");

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

          if (Access::Consult(way) == elle::Status::Error)
            escape("unable to consult");

          break;
        }
      case Access::OperationGrant:
        {
          elle::String                  path;
          elle::String                  string;
          nucleus::neutron::Subject::Type        type;
          nucleus::neutron::Subject              subject;
          nucleus::neutron::Permissions          permissions;

          // retrieve the path.
          if (Infinit::Parser->Value("Path",
                                     path) == elle::Status::Error)
            escape("unable to retrieve the path value");

          // retrieve the type.
          if (Infinit::Parser->Value("Type",
                                     string) == elle::Status::Error)
            escape("unable to retrieve the type value");

          // convert the string into a subject type.
          if (nucleus::neutron::Subject::Convert(string, type) == elle::Status::Error)
            escape("unable to convert the string '%s' into a "
                   "valid subject type",
                   string.c_str());

          // build a subject depending on the type.
          switch (type)
            {
            case nucleus::neutron::Subject::TypeUser:
              {
                elle::cryptography::PublicKey         K;
                std::string res;

                // retrieve the identifier which is supposed to
                // represent a user identity i.e a public key.
                if (Infinit::Parser->Value(
                      "Identifier",
                      res) == elle::Status::Error)
                  escape("unable to retrieve the identifier");

                if (K.Restore(res) == elle::Status::Error)
                  escape("unable to retrieve the user's public key "
                         "through the identifier");

                // build the subject.
                if (subject.Create(K) == elle::Status::Error)
                  escape("unable to create the subject");

                break;
              }
            case nucleus::neutron::Subject::TypeGroup:
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
          permissions = nucleus::neutron::PermissionNone;

          // grant the read permission, if requested.
          if (Infinit::Parser->Test("Read") == elle::Status::True)
            permissions |= nucleus::neutron::PermissionRead;

          // grant the write permission, if requested.
          if (Infinit::Parser->Test("Write") == elle::Status::True)
            permissions |= nucleus::neutron::PermissionWrite;

          // declare additional local variables.
          etoile::path::Way             way(path);

          if (Access::Grant(way, subject, permissions) == elle::Status::Error)
            escape("unable to grant");

          break;
        }
      case Access::OperationRevoke:
        {
          elle::String                  path;
          elle::String                  string;
          nucleus::neutron::Subject::Type        type;
          nucleus::neutron::Subject              subject;

          // retrieve the path.
          if (Infinit::Parser->Value("Path",
                                     path) == elle::Status::Error)
            escape("unable to retrieve the path value");

          // retrieve the type.
          if (Infinit::Parser->Value("Type",
                                     string) == elle::Status::Error)
            escape("unable to retrieve the type value");

          // convert the string into a subject type.
          if (nucleus::neutron::Subject::Convert(string, type) == elle::Status::Error)
            escape("unable to convert the string '%s' into a "
                   "valid subject type",
                   string.c_str());

          // build a subject depending on the type.
          switch (type)
            {
            case nucleus::neutron::Subject::TypeUser:
              {
                elle::cryptography::PublicKey         K;
                std::string res;

                // retrieve the identifier which is supposed to
                // represent a user identity i.e a public key.
                if (Infinit::Parser->Value(
                      "Identifier",
                      res) == elle::Status::Error)
                  escape("unable to retrieve the identifier");

                if (K.Restore(res) == elle::Status::Error)
                  escape("unable to retrieve the user's public key "
                         "through the identifier");

                // build the subject.
                if (subject.Create(K) == elle::Status::Error)
                  escape("unable to create the subject");

                break;
              }
            case nucleus::neutron::Subject::TypeGroup:
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

          if (Access::Revoke(way, subject) == elle::Status::Error)
            escape("unable to revoke");

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

elle::Status
_main(elle::Natural32 argc, elle::Character* argv[])
{
  try
    {
      if (satellite::Main(argc, argv) == elle::Status::Error)
        throw reactor::Exception(elle::concurrency::scheduler(),
                                 "XXX");
    }
  catch (std::exception const& e)
    {
      // XXX
      show();

      std::cerr << argv[0] << ": fatal error: " << e.what() << std::endl;
      if (reactor::Exception const* re =
          dynamic_cast<reactor::Exception const*>(&e))
        std::cerr << re->backtrace() << std::endl;

      elle::concurrency::scheduler().terminate();
      return elle::Status::Error;
    }
  elle::concurrency::scheduler().terminate();
  return elle::Status::Ok;
}

///
/// this is the program entry point.
///
int                     main(int                                argc,
                             char**                             argv)
{
  reactor::Scheduler& sched = elle::concurrency::scheduler();
  reactor::VThread<elle::Status> main(sched, "main",
                                      boost::bind(&_main, argc, argv));
  sched.run();
  return main.result() == elle::Status::Ok ? 0 : 1;
}
