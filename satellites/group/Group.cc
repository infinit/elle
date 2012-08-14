#include <satellites/group/Group.hh>

#include <elle/Elle.hh>
#include <elle/network/TCPSocket.hh>
#include <elle/network/Inputs.hh>
#include <elle/network/Outputs.hh>
#include <elle/utility/Parser.hh>
#include <elle/standalone/Report.hh>
#include <elle/concurrency/Program.hh>
#include <elle/io/Piece.hh>
#include <elle/io/Path.hh>
#include <elle/io/Unique.hh>

#include <etoile/gear/Identifier.hh>
#include <etoile/portal/Manifest.hh>
#include <etoile/abstract/Group.hh>

#include <nucleus/Nucleus.hh>
#include <nucleus/neutron/Range.hh>
#include <nucleus/neutron/Fellow.hh>
#include <nucleus/neutron/Subject.hh>

#include <lune/Lune.hh>
#include <lune/Phrase.hh>

#include <hole/Hole.hh>

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
  const elle::Character         Component[] = "8group";

  ///
  /// the socket used for communicating with Etoile.
  ///
  elle::network::TCPSocket* Group::socket = nullptr;

//
// ---------- static methods --------------------------------------------------
//

  elle::Status
  Group::Display(nucleus::neutron::Fellow const& fellow)
  {
    switch (fellow.subject().type())
      {
      case nucleus::neutron::Subject::TypeUser:
        {
          elle::io::Unique unique;

          // convert the public key into a human-kind-of-readable string.
          if (fellow.subject().user().Save(unique) == elle::Status::Error)
            escape("unable to save the public key's unique");

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
            escape("unable to save the address' unique");

          std::cout << "Group"
                    << " "
                    << unique
                    << std::endl;

          break;
        }
      default:
        {
          escape("unknown subject type '%u'", fellow.subject().type());
        }
      }

    return (elle::Status::Ok);
  }

  elle::Status
  Group::Connect()
  {
    lune::Phrase phrase;

    // load the phrase.
    if (phrase.Load(
          elle::io::Path(lune::Lune::Network::Phrase,
                         elle::io::Piece("%NETWORK%", Infinit::Network),
                         elle::io::Piece("%NAME%", "portal"))) == elle::Status::Error)
      escape("unable to load the phrase");

    // connect to the server.
    reactor::network::TCPSocket* socket =
      new reactor::network::TCPSocket(elle::concurrency::scheduler(),
                                      elle::String("127.0.0.1"),
                                      phrase.port);

    Group::socket = new elle::network::TCPSocket(socket, false);

    // authenticate.
    if (Group::socket->Call(
          elle::network::Inputs<etoile::portal::TagAuthenticate>(phrase.pass),
          elle::network::Outputs<etoile::portal::TagAuthenticated>()) ==
        elle::Status::Error)
      escape("unable to authenticate to Etoile");

    return (elle::Status::Ok);
  }

  elle::Status
  Group::Information(typename nucleus::neutron::Group::Identity const& identity)
  {
    etoile::gear::Identifier identifier;
    etoile::abstract::Group abstract;

    // connect to Etoile.
    if (Group::Connect() == elle::Status::Error)
      goto _error;

    // load the object.
    if (Group::socket->Call(
          elle::network::Inputs<etoile::portal::TagGroupLoad>(identity),
          elle::network::Outputs<etoile::portal::TagIdentifier>(identifier)) ==
        elle::Status::Error)
      goto _error;

    // retrieve the group abstract.
    if (Group::socket->Call(
          elle::network::Inputs<etoile::portal::TagGroupInformation>(
            identifier),
          elle::network::Outputs<etoile::portal::TagGroupAbstract>(abstract)) ==
        elle::Status::Error)
      goto _error;

    // discard the group.
    if (Group::socket->Call(
          elle::network::Inputs<etoile::portal::TagGroupDiscard>(identifier),
          elle::network::Outputs<elle::TagOk>()) == elle::Status::Error)
      goto _error;

    // dump the abstract.
    if (abstract.Dump() == elle::Status::Error)
      goto _error;

    return elle::Status::Ok;

  _error:
    // release the object.
    if (Group::socket != nullptr)
      {
        Group::socket->send(
          elle::network::Inputs<etoile::portal::TagObjectDiscard>(
            identifier));
      }

    // expose the potential errors.
    expose();

    // exit the program.
    elle::concurrency::Program::Exit();

    return elle::Status::Ok;
  }

  elle::Status
  Group::Create(elle::String const& description)
  {
    typename nucleus::neutron::Group::Identity identity;
    etoile::gear::Identifier identifier;
    elle::io::Unique unique;

    // connect to Etoile.
    if (Group::Connect() == elle::Status::Error)
      goto _error;

    // load the object.
    if (Group::socket->Call(
          elle::network::Inputs<etoile::portal::TagGroupCreate>(description),
          elle::network::Outputs<etoile::portal::TagGroupIdentity>(
            identity, identifier)) == elle::Status::Error)
      goto _error;

    // store the object.
    if (Group::socket->Call(
          elle::network::Inputs<etoile::portal::TagGroupStore>(identifier),
          elle::network::Outputs<elle::TagOk>()) == elle::Status::Error)
      goto _error;

    // display the group identity.
    if (identity.Save(unique) == elle::Status::Error)
      goto _error;

    std::cout << unique << std::endl;

    return elle::Status::Ok;

  _error:
    // release the object.
    if (Group::socket != nullptr)
      {
        Group::socket->send(
          elle::network::Inputs<etoile::portal::TagObjectDiscard>(
            identifier));
      }

    // expose the potential errors.
    expose();

    // exit the program.
    elle::concurrency::Program::Exit();

    return elle::Status::Ok;
  }

  elle::Status
  Group::Add(typename nucleus::neutron::Group::Identity const& identity,
             nucleus::neutron::Subject const& subject)
  {
    etoile::gear::Identifier identifier;

    // connect to Etoile.
    if (Group::Connect() == elle::Status::Error)
      goto _error;

    // load the object.
    if (Group::socket->Call(
          elle::network::Inputs<etoile::portal::TagGroupLoad>(identity),
          elle::network::Outputs<etoile::portal::TagIdentifier>(identifier)) ==
        elle::Status::Error)
      goto _error;

    // add the subject.
    if (Group::socket->Call(
          elle::network::Inputs<etoile::portal::TagGroupAdd>(identifier,
                                                             subject),
          elle::network::Outputs<elle::TagOk>()) == elle::Status::Error)
      goto _error;

    // store the group.
    if (Group::socket->Call(
          elle::network::Inputs<etoile::portal::TagGroupStore>(identifier),
          elle::network::Outputs<elle::TagOk>()) == elle::Status::Error)
      goto _error;

    return elle::Status::Ok;

  _error:
    // release the object.
    if (Group::socket != nullptr)
      {
        Group::socket->send(
          elle::network::Inputs<etoile::portal::TagObjectDiscard>(
            identifier));
      }

    // expose the potential errors.
    expose();

    // exit the program.
    elle::concurrency::Program::Exit();

    return elle::Status::Ok;
  }

  elle::Status
  Group::Lookup(typename nucleus::neutron::Group::Identity const& identity,
                nucleus::neutron::Subject const& subject)
  {
    etoile::gear::Identifier identifier;
    nucleus::neutron::Fellow fellow;

    // connect to Etoile.
    if (Group::Connect() == elle::Status::Error)
      goto _error;

    // load the object.
    if (Group::socket->Call(
          elle::network::Inputs<etoile::portal::TagGroupLoad>(identity),
          elle::network::Outputs<etoile::portal::TagIdentifier>(identifier)) ==
        elle::Status::Error)
      goto _error;

    // lookup the subject.
    if (Group::socket->Call(
          elle::network::Inputs<etoile::portal::TagGroupLookup>(identifier,
                                                                subject),
          elle::network::Outputs<etoile::portal::TagGroupFellow>(fellow)) == elle::Status::Error)
      goto _error;

    // discard the group.
    if (Group::socket->Call(
          elle::network::Inputs<etoile::portal::TagGroupDiscard>(identifier),
          elle::network::Outputs<elle::TagOk>()) == elle::Status::Error)
      goto _error;

    if (Group::Display(fellow) == elle::Status::Error)
      goto _error;

    return elle::Status::Ok;

  _error:
    // release the object.
    if (Group::socket != nullptr)
      {
        Group::socket->send(
          elle::network::Inputs<etoile::portal::TagObjectDiscard>(
            identifier));
      }

    // expose the potential errors.
    expose();

    // exit the program.
    elle::concurrency::Program::Exit();

    return elle::Status::Ok;
  }

  elle::Status
  Group::Consult(typename nucleus::neutron::Group::Identity const& identity)
  {
    etoile::gear::Identifier identifier;
    nucleus::neutron::Range<nucleus::neutron::Fellow> range;
    nucleus::neutron::Index index(0);
    nucleus::neutron::Size size(std::numeric_limits<nucleus::neutron::Size>::max());

    // connect to Etoile.
    if (Group::Connect() == elle::Status::Error)
      goto _error;

    // load the object.
    if (Group::socket->Call(
          elle::network::Inputs<etoile::portal::TagGroupLoad>(identity),
          elle::network::Outputs<etoile::portal::TagIdentifier>(identifier)) ==
        elle::Status::Error)
      goto _error;

    // consult the group.
    if (Group::socket->Call(
          elle::network::Inputs<etoile::portal::TagGroupConsult>(
            identifier, index, size),
          elle::network::Outputs<etoile::portal::TagGroupRange>(range)) == elle::Status::Error)
      goto _error;

    // discard the group.
    if (Group::socket->Call(
          elle::network::Inputs<etoile::portal::TagGroupDiscard>(identifier),
          elle::network::Outputs<elle::TagOk>()) == elle::Status::Error)
      goto _error;

    for (auto fellow: range.container)
      {
        if (Group::Display(*fellow) == elle::Status::Error)
          goto _error;
      }

    return elle::Status::Ok;

  _error:
    // release the object.
    if (Group::socket != nullptr)
      {
        Group::socket->send(
          elle::network::Inputs<etoile::portal::TagObjectDiscard>(
            identifier));
      }

    // expose the potential errors.
    expose();

    // exit the program.
    elle::concurrency::Program::Exit();

    return elle::Status::Ok;
  }

  elle::Status
  Group::Remove(typename nucleus::neutron::Group::Identity const& identity,
                nucleus::neutron::Subject const& subject)
  {
    etoile::gear::Identifier identifier;

    // connect to Etoile.
    if (Group::Connect() == elle::Status::Error)
      goto _error;

    // load the object.
    if (Group::socket->Call(
          elle::network::Inputs<etoile::portal::TagGroupLoad>(identity),
          elle::network::Outputs<etoile::portal::TagIdentifier>(identifier)) ==
        elle::Status::Error)
      goto _error;

    // remove the subject.
    if (Group::socket->Call(
          elle::network::Inputs<etoile::portal::TagGroupRemove>(identifier,
                                                                subject),
          elle::network::Outputs<elle::TagOk>()) == elle::Status::Error)
      goto _error;

    // store the group.
    if (Group::socket->Call(
          elle::network::Inputs<etoile::portal::TagGroupStore>(identifier),
          elle::network::Outputs<elle::TagOk>()) == elle::Status::Error)
      goto _error;

    return elle::Status::Ok;

  _error:
    // release the object.
    if (Group::socket != nullptr)
      {
        Group::socket->send(
          elle::network::Inputs<etoile::portal::TagObjectDiscard>(
            identifier));
      }

    // expose the potential errors.
    expose();

    // exit the program.
    elle::concurrency::Program::Exit();

    return elle::Status::Ok;
  }

  elle::Status
  Group::Destroy(typename nucleus::neutron::Group::Identity const& identity)
  {
    etoile::gear::Identifier identifier;

    // connect to Etoile.
    if (Group::Connect() == elle::Status::Error)
      goto _error;

    // load the object.
    if (Group::socket->Call(
          elle::network::Inputs<etoile::portal::TagGroupLoad>(identity),
          elle::network::Outputs<etoile::portal::TagIdentifier>(identifier)) ==
        elle::Status::Error)
      goto _error;

    // destroy the group.
    if (Group::socket->Call(
          elle::network::Inputs<etoile::portal::TagGroupDestroy>(identifier),
          elle::network::Outputs<elle::TagOk>()) == elle::Status::Error)
      goto _error;

    return elle::Status::Ok;

  _error:
    // release the object.
    if (Group::socket != nullptr)
      {
        Group::socket->send(
          elle::network::Inputs<etoile::portal::TagObjectDiscard>(
            identifier));
      }

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
    Group::Operation   operation;

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

    // initialize hole.
    hole::Hole::Initialize();

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
              group = hole::Hole::Descriptor.everybody;
            }

          if (Group::Information(group) == elle::Status::Error)
            escape("unable to retrieve information on the group");

          break;
        }
      case Group::OperationCreate:
        {
          elle::String description;

          // retrieve the description.
          if (Infinit::Parser->Value("Description",
                                     description) == elle::Status::Error)
            escape("unable to retrieve the description");

          if (Group::Create(description) == elle::Status::Error)
            escape("unable to create the group");

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
              group = hole::Hole::Descriptor.everybody;
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

          if (Group::Add(group, subject) == elle::Status::Error)
            escape("unable to add the subject into the group");

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
              group = hole::Hole::Descriptor.everybody;
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

          if (Group::Lookup(group, subject) == elle::Status::Error)
            escape("unable to lookup the subject in the group");

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
              group = hole::Hole::Descriptor.everybody;
            }

          if (Group::Consult(group) == elle::Status::Error)
            escape("unable to consult the group's fellows");

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
              group = hole::Hole::Descriptor.everybody;
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

          if (Group::Remove(group, subject) == elle::Status::Error)
            escape("unable to remove the subject from the group");

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
              group = hole::Hole::Descriptor.everybody;
            }

          if (Group::Destroy(group) == elle::Status::Error)
            escape("unable to destroy the group");

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

    // clean hole.
    hole::Hole::Clean();

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
