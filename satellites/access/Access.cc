#include <satellites/access/Access.hh>

#include <elle/utility/Parser.hh>
#include <elle/concurrency/Program.hh>
#include <elle/io/Piece.hh>
#include <elle/io/Path.hh>

#include <reactor/network/tcp-socket.hh>

#include <protocol/Serializer.hh>

#include <etoile/gear/Identifier.hh>
#include <etoile/path/Chemin.hh>
#include <etoile/path/Way.hh>
#include <etoile/portal/Manifest.hh>

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
  reactor::network::TCPSocket* Access::socket = nullptr;
  infinit::protocol::Serializer* Access::serializer = nullptr;
  infinit::protocol::ChanneledStream* Access::channels = nullptr;
  etoile::portal::RPC* Access::rpcs = nullptr;

  /// Ward helper to make sure objects are discarded on errors.
  class Ward
  {
  public:
    Ward(etoile::gear::Identifier const& id):
      _id(id),
      _clean(true)
    {}

    ~Ward()
    {
      if (_clean && Access::socket != nullptr)
        Access::rpcs->objectdiscard(this->_id);
    }

    void release()
    {
      _clean = false;
    }

  private:
    etoile::gear::Identifier _id;
    bool _clean;
  };

  void
  display(nucleus::neutron::Record const& record)
  {
    switch (record.subject().type())
      {
        case nucleus::neutron::Subject::TypeUser:
        {
          elle::io::Unique unique;
          // Convert the public key into a human-kind-of-readable string.
          if (record.subject().user().Save(unique) == elle::Status::Error)
            throw reactor::Exception(elle::concurrency::scheduler(),
                                     "unable to save the public key's unique");
          std::cout << "User"
                    << unique
                    << " "
                    << std::dec
                    << static_cast<elle::Natural32>(record.permissions())
                    << std::endl;
          break;
        }
        case nucleus::neutron::Subject::TypeGroup:
        {
          elle::io::Unique unique;
          // Convert the group's address into a human-kind-of-readable string.
          if (record.subject().group().Save(unique) == elle::Status::Error)
            throw reactor::Exception(elle::concurrency::scheduler(),
                                     "unable to save the address' unique");

          std::cout << "Group"
                    << " "
                    << unique
                    << " "
                    << std::dec << static_cast<elle::Natural32>(record.permissions()) << std::endl;

          break;
        }
        default:
        {
          break;
        }
      }
  }

  void
  Access::connect()
  {
    // Load the phrase.
    lune::Phrase        phrase;
    phrase.load(Infinit::Network, "portal");

    // Connect to the server.
    Access::socket =
      new reactor::network::TCPSocket(elle::concurrency::scheduler(),
                                      elle::String("127.0.0.1"),
                                      phrase.port);
    Access::serializer =
      new infinit::protocol::Serializer(elle::concurrency::scheduler(), *socket);
    Access::channels =
      new infinit::protocol::ChanneledStream(elle::concurrency::scheduler(),
                                             *serializer, true);
    Access::rpcs = new etoile::portal::RPC(*channels);

    if (!Access::rpcs->authenticate(phrase.pass))
      throw reactor::Exception(elle::concurrency::scheduler(),
                               "unable to authenticate to Etoile");
  }

  void
  Access::lookup(const etoile::path::Way& way,
                 const nucleus::neutron::Subject& subject)
  {
    Access::connect();
    // Resolve the path.
    etoile::path::Chemin chemin(Access::rpcs->pathresolve(way));
    // Load the object.
    etoile::gear::Identifier identifier(Access::rpcs->objectload(chemin));
    Ward ward(identifier);
    // Lookup the access record.
    nucleus::neutron::Record record(
      Access::rpcs->accesslookup(identifier, subject));
    display(record);
  }

  void
  Access::consult(const etoile::path::Way& way)
  {
    Access::connect();
    // Resolve the path.
    etoile::path::Chemin chemin(Access::rpcs->pathresolve(way));
    // Load the object.
    etoile::gear::Identifier identifier(Access::rpcs->objectload(chemin));
    Ward ward(identifier);
    // Consult the object access.
    nucleus::neutron::Range<nucleus::neutron::Record> records(
      Access::rpcs->accessconsult(identifier,
                                  std::numeric_limits<nucleus::neutron::Index>::min(),
                                  std::numeric_limits<nucleus::neutron::Size>::max()));
    // Dump the records.
    BOOST_FOREACH(auto record, records.container)
      display(*record);
  }

  void
  Access::grant(const etoile::path::Way&  way,
                const nucleus::neutron::Subject&   subject,
                const nucleus::neutron::Permissions permissions)
  {
    Access::connect();
    // Resolve the path.
    etoile::path::Chemin chemin(Access::rpcs->pathresolve(way));
    // Load the object.
    etoile::gear::Identifier identifier(Access::rpcs->objectload(chemin));
    Ward ward(identifier);
    // Lookup the access record.
    Access::rpcs->accessgrant(identifier, subject, permissions);
    // store the object.
    Access::rpcs->objectstore(identifier);
    ward.release();
  }

  void
  Access::revoke(const etoile::path::Way& way,
                 const nucleus::neutron::Subject&  subject)
  {
    Access::connect();
    // Resolve the path.
    etoile::path::Chemin chemin(Access::rpcs->pathresolve(way));
    // Load the object.
    etoile::gear::Identifier identifier(Access::rpcs->objectload(chemin));
    Ward ward(identifier);
    // Revoke the access for the given subject.
    Access::rpcs->accessrevoke(identifier, subject);
    // store the object.
    Access::rpcs->objectstore(identifier);
    ward.release();
  }

  elle::Status          Main(elle::Natural32                    argc,
                             elle::Character*                   argv[])
  {
    Access::Operation   operation;

    // XXX Infinit::Parser is not deleted in case of errors

    // set up the program.
    if (elle::concurrency::Program::Setup() == elle::Status::Error)
      escape("unable to set up the program");

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
          "Identity",
          'i',
          "identity",
          "specify the user/group base64 identity",
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

                // retrieve the identity which is supposed to
                // represent a user identity i.e a public key.
                if (Infinit::Parser->Value(
                      "Identity",
                      res) == elle::Status::Error)
                  escape("unable to retrieve the identity");

                if (K.Restore(res) == elle::Status::Error)
                  escape("unable to retrieve the user's public key "
                         "through the identity");

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

          Access::lookup(way, subject);

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

          Access::consult(way);
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

                // retrieve the identity which is supposed to
                // represent a user identity i.e a public key.
                if (Infinit::Parser->Value(
                      "Identity",
                      res) == elle::Status::Error)
                  escape("unable to retrieve the identity");

                if (K.Restore(res) == elle::Status::Error)
                  escape("unable to retrieve the user's public key "
                         "through the identity");

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
          permissions = nucleus::neutron::permissions::none;

          // grant the read permission, if requested.
          if (Infinit::Parser->Test("Read") == elle::Status::True)
            permissions |= nucleus::neutron::permissions::read;

          // grant the write permission, if requested.
          if (Infinit::Parser->Test("Write") == elle::Status::True)
            permissions |= nucleus::neutron::permissions::write;

          // declare additional local variables.
          etoile::path::Way             way(path);

          Access::grant(way, subject, permissions);
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

                // retrieve the identity which is supposed to
                // represent a user identity i.e a public key.
                if (Infinit::Parser->Value(
                      "Identity",
                      res) == elle::Status::Error)
                  escape("unable to retrieve the identity");

                if (K.Restore(res) == elle::Status::Error)
                  escape("unable to retrieve the user's public key "
                         "through the identity");

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

          Access::revoke(way, subject);
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
