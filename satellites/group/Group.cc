#include <satellites/group/Group.hh>

#include <elle/Elle.hh>
#include <elle/utility/Parser.hh>
#include <elle/standalone/Report.hh>
#include <elle/concurrency/Program.hh>
#include <elle/io/Piece.hh>
#include <elle/io/Path.hh>
#include <elle/io/Unique.hh>

#include <reactor/exception.hh>
#include <reactor/network/tcp-socket.hh>

#include <protocol/Serializer.hh>

#include <elle/serialize/PairSerializer.hxx>

#include <etoile/gear/Identifier.hh>
#include <etoile/portal/Manifest.hh>
#include <etoile/abstract/Group.hh>

#include <nucleus/neutron/Range.hh>
#include <nucleus/neutron/Fellow.hh>
#include <nucleus/neutron/Subject.hh>

#include <lune/Descriptor.hh>
#include <lune/Lune.hh>
#include <lune/Phrase.hh>

#include <hole/Hole.hh>
#include <hole/storage/Directory.hh>

#include <elle/idiom/Close.hh>
# include <boost/foreach.hpp>
# include <limits>
#include <elle/idiom/Open.hh>

ELLE_LOG_COMPONENT("infinit.8group");

namespace satellite
{
  reactor::network::TCPSocket* Group::socket = nullptr;
  infinit::protocol::Serializer* Group::serializer = nullptr;
  infinit::protocol::ChanneledStream* Group::channels = nullptr;
  etoile::portal::RPC* Group::rpcs = nullptr;

  /// Ward helper to make sure groups are discarded on errors.
  class Ward
  {
  public:
    Ward(etoile::gear::Identifier const& id):
      _id(id),
      _clean(true)
    {}

    ~Ward()
    {
      if (_clean && Group::socket != nullptr)
        Group::rpcs->groupdiscard(this->_id);
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
  Group::display(nucleus::neutron::Fellow const& fellow)
  {
    switch (fellow.subject().type())
      {
      case nucleus::neutron::Subject::TypeUser:
        {
          elle::io::Unique unique;

          // convert the public key into a human-kind-of-readable string.
          if (fellow.subject().user().Save(unique) == elle::Status::Error)
            throw reactor::Exception(elle::concurrency::scheduler(),
                                     "unable to save the public key's unique");

          std::cout << "User"
                    << " "
                    << unique
                    << std::endl;

          break;
        }
      case nucleus::neutron::Subject::TypeGroup:
        {
          elle::io::Unique unique;

          // convert the group's address into a human-kind-of-readable string.
          if (fellow.subject().group().Save(unique) == elle::Status::Error)
            throw reactor::Exception(elle::concurrency::scheduler(),
                                     "unable to save the address' unique");

          std::cout << "Group"
                    << " "
                    << unique
                    << std::endl;

          break;
        }
      default:
        {
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   elle::sprintf("unknown subject type '%u'",
                                                 fellow.subject().type()));
        }
      }
  }

  void
  Group::connect()
  {
    lune::Phrase phrase;

    // Load the phrase.
    phrase.load(Infinit::Network, "portal");

    Group::socket =
      new reactor::network::TCPSocket(elle::concurrency::scheduler(),
                                      elle::String("127.0.0.1"),
                                      phrase.port);
    Group::serializer =
      new infinit::protocol::Serializer(elle::concurrency::scheduler(), *socket);
    Group::channels =
      new infinit::protocol::ChanneledStream(elle::concurrency::scheduler(),
                                             *serializer, true);
    Group::rpcs = new etoile::portal::RPC(*channels);

    // Authenticate.
    if (!Group::rpcs->authenticate(phrase.pass))
      throw reactor::Exception(elle::concurrency::scheduler(),
                               "authentication failed");
  }

  void
  Group::information(typename nucleus::neutron::Group::Identity const& identity)
  {
    Group::connect();
    etoile::gear::Identifier identifier = Group::rpcs->groupload(identity);
    Ward ward(identifier);
    etoile::abstract::Group abstract = Group::rpcs->groupinformation(identifier);
    abstract.Dump();
  }

  void
  Group::create(elle::String const& description)
  {
    Group::connect();
    auto group = Group::rpcs->groupcreate(description);
    auto identity = group.first;
    etoile::gear::Identifier identifier = group.second;
    Group::rpcs->groupstore(identifier);
    elle::io::Unique unique;
    if (identity.Save(unique) == elle::Status::Error)
      throw reactor::Exception(elle::concurrency::scheduler(),
                               "unable to save the identity");
    std::cout << unique << std::endl;

  }

  void
  Group::add(typename nucleus::neutron::Group::Identity const& identity,
             nucleus::neutron::Subject const& subject)
  {
    Group::connect();
    etoile::gear::Identifier identifier = Group::rpcs->groupload(identity);
    Ward ward(identifier);
    Group::rpcs->groupadd(identifier, subject);
    Group::rpcs->groupstore(identifier);
    ward.release();
  }

  void
  Group::lookup(typename nucleus::neutron::Group::Identity const& identity,
                nucleus::neutron::Subject const& subject)
  {
    Group::connect();
    etoile::gear::Identifier identifier = Group::rpcs->groupload(identity);
    Ward ward(identifier);
    nucleus::neutron::Fellow fellow(
      Group::rpcs->grouplookup(identifier, subject));
    Group::display(fellow);
  }

  void
  Group::consult(typename nucleus::neutron::Group::Identity const& identity)
  {
    nucleus::neutron::Index index(0);
    nucleus::neutron::Size size(std::numeric_limits<nucleus::neutron::Size>::max());

    Group::connect();
    etoile::gear::Identifier identifier = Group::rpcs->groupload(identity);
    Ward ward(identifier);
    nucleus::neutron::Range<nucleus::neutron::Fellow> fellows =
      Group::rpcs->groupconsult(identifier, index, size);
    for (auto fellow: fellows.container)
      Group::display(*fellow);
  }

  void
  Group::remove(typename nucleus::neutron::Group::Identity const& identity,
                nucleus::neutron::Subject const& subject)
  {
    Group::connect();
    etoile::gear::Identifier identifier = Group::rpcs->groupload(identity);
    Ward ward(identifier);
    Group::rpcs->groupremove(identifier, subject);
    Group::rpcs->groupstore(identifier);
    ward.release();
  }

  void
  Group::destroy(typename nucleus::neutron::Group::Identity const& identity)
  {
    Group::connect();
    etoile::gear::Identifier identifier = Group::rpcs->groupload(identity);
    Ward ward(identifier);
    Group::rpcs->groupdestroy(identifier);
    ward.release();
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
    // FIXME: do not re-parse the descriptor every time.
    lune::Descriptor descriptor(Infinit::Network);

    Group::Operation   operation;

    // XXX Infinit::Parser is not deleted in case of errors

    // initialize the Elle library.
    if (elle::Elle::Initialize() == elle::Status::Error)
      escape("unable to initialize Elle");

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
    operation = Group::OperationUnknown;

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
          "Information",
          'x',
          "information",
          "display information on a group",
          elle::utility::Parser::KindNone) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Create",
          'c',
          "create",
          "create a group",
          elle::utility::Parser::KindNone) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Description",
          'd',
          "description",
          "specify the group description on creation",
          elle::utility::Parser::KindRequired) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Lookup",
          'l',
          "lookup",
          "look up a specific group record",
          elle::utility::Parser::KindNone) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Consult",
          'o',
          "consult",
          "consult the group records",
          elle::utility::Parser::KindNone) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Add",
          'a',
          "add",
          "add a user/group to a group",
          elle::utility::Parser::KindNone) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Remove",
          'r',
          "remove",
          "remove a user/group from a group",
          elle::utility::Parser::KindNone) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Destroy",
          'y',
          "destroy",
          "destroy the given group",
          elle::utility::Parser::KindNone) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Group",
          'g',
          "group",
          "indicate the group base64 identity on which to operate",
          elle::utility::Parser::KindOptional) == elle::Status::Error)
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
          "Identity",
          'i',
          "identity",
          "specify the user/group base64 identity to add/remove/lookup in the group",
          elle::utility::Parser::KindRequired) == elle::Status::Error)
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

    // Instanciate a Storage
    elle::io::Path shelter(lune::Lune::Network::Shelter::Root);
    shelter.Complete(elle::io::Piece("%NETWORK%", Infinit::Network));
    hole::storage::Directory storage(shelter.string());

    std::unique_ptr<hole::Hole> hole(hole::factory(storage));

    // check the mutually exclusive options.
    if ((Infinit::Parser->Test("Information") == elle::Status::True) &&
        (Infinit::Parser->Test("Create") == elle::Status::True) &&
        (Infinit::Parser->Test("Add") == elle::Status::True) &&
        (Infinit::Parser->Test("Lookup") == elle::Status::True) &&
        (Infinit::Parser->Test("Consult") == elle::Status::True) &&
        (Infinit::Parser->Test("Remove") == elle::Status::True) &&
        (Infinit::Parser->Test("Destroy") == elle::Status::True))
      {
        // display the usage.
        Infinit::Parser->Usage();

        escape("the information, create, add, lookup, consult, remove and "
               "destroy options are mutually exclusive");
      }

    // test the options.
    if (Infinit::Parser->Test("Information") == elle::Status::True)
      operation = Group::OperationInformation;

    if (Infinit::Parser->Test("Create") == elle::Status::True)
      operation = Group::OperationCreate;

    if (Infinit::Parser->Test("Add") == elle::Status::True)
      operation = Group::OperationAdd;

    if (Infinit::Parser->Test("Lookup") == elle::Status::True)
      operation = Group::OperationLookup;

    if (Infinit::Parser->Test("Consult") == elle::Status::True)
      operation = Group::OperationConsult;

    if (Infinit::Parser->Test("Remove") == elle::Status::True)
      operation = Group::OperationRemove;

    if (Infinit::Parser->Test("Destroy") == elle::Status::True)
      operation = Group::OperationDestroy;

    // trigger the operation.
    switch (operation)
      {
      case Group::OperationInformation:
        {
          elle::String string;
          typename nucleus::neutron::Group::Identity group;

          // retrieve the group.
          if (Infinit::Parser->Value("Group",
                                     string,
                                     elle::String()) == elle::Status::Error)
            escape("unable to retrieve the group identity");

          // if no group is provided, use the "everybody" group of the network.
          if (string.empty() == false)
            {
              // convert the string into a group identity.
              if (group.Restore(string) == elle::Status::Error)
                escape("unable to convert the string into a group identity");
            }
          else
            {

              group = descriptor.meta().everybody_identity();
            }

          Group::information(group);

          break;
        }
      case Group::OperationCreate:
        {
          elle::String description;

          // retrieve the description.
          if (Infinit::Parser->Value("Description",
                                     description) == elle::Status::Error)
            escape("unable to retrieve the description");

          Group::create(description);

          break;
        }
      case Group::OperationAdd:
        {
          elle::String string;
          typename nucleus::neutron::Group::Identity group;
          nucleus::neutron::Subject::Type type;
          nucleus::neutron::Subject subject;

          // retrieve the group.
          if (Infinit::Parser->Value("Group",
                                     string,
                                     elle::String()) == elle::Status::Error)
            escape("unable to retrieve the group identity");

          // if no group is provided, use the "everybody" group of the network.
          if (string.empty() == false)
            {
              // convert the string into a group identity.
              if (group.Restore(string) == elle::Status::Error)
                escape("unable to convert the string into a group identity");
            }
          else
            {
              group = descriptor.meta().everybody_identity();
            }

          // retrieve the type.
          if (Infinit::Parser->Value("Type", string) == elle::Status::Error)
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
                typename nucleus::neutron::User::Identity K;

                // retrieve the identifier which is supposed to
                // represent a user identity i.e a public key.
                if (Infinit::Parser->Value("Identity",
                                           string) == elle::Status::Error)
                  escape("unable to retrieve the identifier");

                if (K.Restore(string) == elle::Status::Error)
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

          Group::add(group, subject);

          break;
        }
      case Group::OperationLookup:
        {
          elle::String string;
          typename nucleus::neutron::Group::Identity group;
          nucleus::neutron::Subject::Type type;
          nucleus::neutron::Subject subject;

          // retrieve the group.
          if (Infinit::Parser->Value("Group",
                                     string,
                                     elle::String()) == elle::Status::Error)
            escape("unable to retrieve the group identity");

          // if no group is provided, use the "everybody" group of the network.
          if (string.empty() == false)
            {
              // convert the string into a group identity.
              if (group.Restore(string) == elle::Status::Error)
                escape("unable to convert the string into a group identity");
            }
          else
            {
              group = descriptor.meta().everybody_identity();
            }

          // retrieve the type.
          if (Infinit::Parser->Value("Type", string) == elle::Status::Error)
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
                typename nucleus::neutron::User::Identity K;

                // retrieve the identifier which is supposed to
                // represent a user identity i.e a public key.
                if (Infinit::Parser->Value("Identity",
                                           string) == elle::Status::Error)
                  escape("unable to retrieve the identifier");

                if (K.Restore(string) == elle::Status::Error)
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

          Group::lookup(group, subject);

          break;
        }
      case Group::OperationConsult:
        {
          elle::String string;
          typename nucleus::neutron::Group::Identity group;

          // retrieve the group.
          if (Infinit::Parser->Value("Group",
                                     string,
                                     elle::String()) == elle::Status::Error)
            escape("unable to retrieve the group identity");

          // if no group is provided, use the "everybody" group of the network.
          if (string.empty() == false)
            {
              // convert the string into a group identity.
              if (group.Restore(string) == elle::Status::Error)
                escape("unable to convert the string into a group identity");
            }
          else
            {
              group = descriptor.meta().everybody_identity();
            }

          Group::consult(group);

          break;
        }
      case Group::OperationRemove:
        {
          elle::String string;
          typename nucleus::neutron::Group::Identity group;
          nucleus::neutron::Subject::Type type;
          nucleus::neutron::Subject subject;

          // retrieve the group.
          if (Infinit::Parser->Value("Group",
                                     string,
                                     elle::String()) == elle::Status::Error)
            escape("unable to retrieve the group identity");

          // if no group is provided, use the "everybody" group of the network.
          if (string.empty() == false)
            {
              // convert the string into a group identity.
              if (group.Restore(string) == elle::Status::Error)
                escape("unable to convert the string into a group identity");
            }
          else
            {
              group = descriptor.meta().everybody_identity();
            }

          // retrieve the type.
          if (Infinit::Parser->Value("Type", string) == elle::Status::Error)
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
                typename nucleus::neutron::User::Identity K;

                // retrieve the identifier which is supposed to
                // represent a user identity i.e a public key.
                if (Infinit::Parser->Value("Identity",
                                           string) == elle::Status::Error)
                  escape("unable to retrieve the identifier");

                if (K.Restore(string) == elle::Status::Error)
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

          Group::remove(group, subject);

          break;
        }
      case Group::OperationDestroy:
        {
          elle::String string;
          typename nucleus::neutron::Group::Identity group;

          // retrieve the group.
          if (Infinit::Parser->Value("Group",
                                     string,
                                     elle::String()) == elle::Status::Error)
            escape("unable to retrieve the group identity");

          // if no group is provided, use the "everybody" group of the network.
          if (string.empty() == false)
            {
              // convert the string into a group identity.
              if (group.Restore(string) == elle::Status::Error)
                escape("unable to convert the string into a group identity");
            }
          else
            {
              group = descriptor.meta().everybody_identity();
            }

          Group::destroy(group);

          break;
        }
      case Group::OperationUnknown:
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

    delete hole.release();

    // clean Infinit.
    if (Infinit::Clean() == elle::Status::Error)
      escape("unable to clean Infinit");

    // clean Lune
    if (lune::Lune::Clean() == elle::Status::Error)
      escape("unable to clean Lune");

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
  catch (reactor::Exception const& e)
    {
      // XXX
      show();

      ELLE_ERR("fatal error: %s", e);
      std::cerr << argv[0] << ": fatal error: " << e.what() << std::endl;
      elle::concurrency::scheduler().terminate();
      return elle::Status::Error;
    }
  catch (std::runtime_error const& e)
    {
      // XXX
      show();

      ELLE_ERR("fatal error: %s", e.what());
      std::cerr << argv[0] << ": fatal error: " << e.what() << std::endl;
      elle::concurrency::scheduler().terminate();
      return elle::Status::Error;
    }
  catch (...)
    {
      // XXX
      show();

      ELLE_ERR("unkown fatal error");
      std::cerr << argv[0] << ": unknown fatal error" << std::endl;
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
