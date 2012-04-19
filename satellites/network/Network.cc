//
// ---------- header ----------------------------------------------------------
//
// project       network
//
// license       infinit
//
// author        julien quintard   [thu mar  4 17:51:46 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <satellites/network/Network.hh>

namespace satellite
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const elle::Character         Component[] = "8network";

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method creates a new network by using the user 'name' as the
  /// initial user.
  ///
  elle::Status          Network::Create(const elle::String&     name,
                                        const hole::Model&      model,
                                        const elle::String&     administrator)
  {
    lune::Authority     authority;
    lune::Descriptor    descriptor;
    lune::Identity      identity;
    nucleus::Object     directory;
    nucleus::Network    network;
    nucleus::Address    address;

    //
    // test the arguments.
    //
    {
      // does the network already exist.
      if (descriptor.Exist(name) == elle::Status::True)
        escape("this network seems to already exist");

      // check the model.
      if (model == hole::Model::Null)
        escape("please specify the model of the network");

      // does the administrator user exist.
      if (identity.Exist(administrator) == elle::Status::False)
        escape("the administrator user does not seem to exist");

      // check if the authority exists.
      if (authority.Exist() == elle::Status::False)
        escape("unable to locate the authority file");
    }

    //
    // retrieve the authority.
    //
    {
      elle::String              prompt;
      elle::String              pass;

      // prompt the user for the passphrase.
      prompt = "Enter passphrase for the authority: ";

      if (elle::Console::Input(
            pass,
            prompt,
            elle::Console::OptionPassword) == elle::Status::Error)
        escape("unable to read the input");

      // load the authority.
      if (authority.Load() == elle::Status::Error)
        escape("unable to load the authority");

      // decrypt the authority.
      if (authority.Decrypt(pass) == elle::Status::Error)
        escape("unable to decrypt the authority");
    }

    //
    // retrieve the administrator's identity.
    //
    {
      elle::String              prompt;
      elle::String              pass;

      // prompt the user for the passphrase.
      prompt = "Enter passphrase for keypair '" + administrator + "': ";

      if (elle::Console::Input(
            pass,
            prompt,
            elle::Console::OptionPassword) == elle::Status::Error)
        escape("unable to read the input");

      // load the identity.
      if (identity.Load(administrator) == elle::Status::Error)
        escape("unable to load the administrator's identity");

      // decrypt the authority.
      if (identity.Decrypt(pass) == elle::Status::Error)
        escape("unable to decrypt the identity");
    }

    //
    // create the root directory.
    //
    {
      // create the network object.
      if (network.Create(name) == elle::Status::Error)
        escape("unable to create the network object");

      // create directory object, setting the user's as the administrator.
      if (directory.Create(nucleus::GenreDirectory,
                           identity.pair.K) == elle::Status::Error)
        escape("unable to create the object directory");

      // seal the directory.
      if (directory.Seal(identity.pair.k,
                         nucleus::Access::Null) == elle::Status::Error)
        escape("unable to seal the object");

      // compute the directory's address.
      if (directory.Bind(address) == elle::Status::Error)
        escape("unable to bind the object to an address");
    }

    //
    // create the network's descriptor.
    //
    {
      // create the descriptor.
      if (descriptor.Create(
            name,
            model,
            address,
            lune::Descriptor::History,
            lune::Descriptor::Extent,
            lune::Descriptor::Contention,
            lune::Descriptor::Balancing) == elle::Status::Error)
        escape("unable to create the network's descriptor");

      // seal the descriptor.
      if (descriptor.Seal(authority) == elle::Status::Error)
        escape("unable to seal the descriptor");

      // store the descriptor.
      if (descriptor.Store(name) == elle::Status::Error)
        escape("unable to store the descriptor file");
    }

    //
    // store the root directory block now that the network exists.
    //
    {
      // store the block.
      if (directory.Store(network,
                          address) == elle::Status::Error)
        escape("unable to store the block");
    }

    return elle::Status::Ok;
  }

  ///
  /// this method destroys an existing network.
  ///
  elle::Status          Network::Destroy(const elle::String&    name)
  {
    //
    // remove the descriptor.
    //
    {
      lune::Descriptor  descriptor;
      elle::Path        path;

      // does the network exist.
      if (descriptor.Exist(name) == elle::Status::True)
        {
          // remove the descriptor.
          if (descriptor.Erase(name) == elle::Status::Error)
            escape("unable to erase the descriptor");
        }
    }

    //
    // destroy the reserve, if necessary
    //
    {
      elle::Path        path;

      // create the reserve path.
      if (path.Create(lune::Lune::Network::Reserve::Root) == elle::Status::Error)
        escape("unable to create the path");

      // complete the path with the network name.
      if (path.Complete(elle::io::Piece("%NETWORK%", name)) == elle::Status::Error)
        escape("unable to complete the path");

      // if the reserve exists, clear it and remove it.
      if (elle::Directory::Exist(path) == elle::Status::True)
        {
          // clear the reserve content.
          if (elle::Directory::Clear(path) == elle::Status::Error)
            escape("unable to clear the directory");

          // remove the directory.
          if (elle::Directory::Remove(path) == elle::Status::Error)
            escape("unable to remove the directory");
        }
    }

    //
    // destroy the shelter.
    //
    {
      elle::Path        path;

      // create the shelter path.
      if (path.Create(lune::Lune::Network::Shelter::Root) == elle::Status::Error)
        escape("unable to create the path");

      // complete the path with the network name.
      if (path.Complete(elle::io::Piece("%NETWORK%", name)) == elle::Status::Error)
        escape("unable to complete the path");

      // if the shelter exists, clear it and remove it.
      if (elle::Directory::Exist(path) == elle::Status::True)
        {
          // clear the reserve content.
          if (elle::Directory::Clear(path) == elle::Status::Error)
            escape("unable to clear the directory");

          // remove the directory.
          if (elle::Directory::Remove(path) == elle::Status::Error)
            escape("unable to remove the directory");
        }
    }

    //
    // remove the network directory.
    //
    {
      elle::Path        path;

      // create the network path.
      if (path.Create(lune::Lune::Network::Root) == elle::Status::Error)
        escape("unable to create the path");

      // complete the path with the network name.
      if (path.Complete(elle::io::Piece("%NETWORK%", name)) == elle::Status::Error)
        escape("unable to complete the path");

      // clear the network directory content.
      if (elle::Directory::Clear(path) == elle::Status::Error)
        escape("unable to clear the directory");

      // remove the directory.
      if (elle::Directory::Remove(path) == elle::Status::Error)
        escape("unable to remove the directory");
    }

    return elle::Status::Ok;
  }

  ///
  /// this method retrieves and displays information on the given network.
  ///
  elle::Status          Network::Information(const elle::String& name)
  {
    lune::Descriptor    descriptor;

    //
    // test the arguments.
    //
    {
      // does the network exist.
      if (descriptor.Exist(name) == elle::Status::False)
        escape("this network does not seem to exist");
    }

    //
    // retrieve the descriptor.
    //
    {
      // load the descriptor.
      if (descriptor.Load(name) == elle::Status::Error)
        escape("unable to load the descriptor");

      // validate the descriptor.
      if (descriptor.Validate(Infinit::Authority) == elle::Status::Error)
        escape("unable to validate the descriptor");
    }

    //
    // display information.
    //
    {
      // dump the descriptor.
      if (descriptor.Dump() == elle::Status::Error)
        escape("unable to dump the descriptor");
    }

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
    Network::Operation  operation;

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
    operation = Network::OperationUnknown;

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

    // register the options.
    if (Infinit::Parser->Register(
          "Create",
          'c',
          "create",
          "create a new network",
          elle::Parser::KindNone) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Destroy",
          'd',
          "destroy",
          "destroy an existing network",
          elle::Parser::KindNone) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Information",
          'x',
          "information",
          "display information regarding a network",
          elle::Parser::KindNone) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Name",
          'n',
          "name",
          "specify the network name",
          elle::Parser::KindRequired) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Model",
          'm',
          "model",
          "specify the network model: Local, Remote, Kool etc.",
          elle::Parser::KindRequired) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Administrator",
          'a',
          "administrator",
          "specify the network administrator",
          elle::Parser::KindRequired) == elle::Status::Error)
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

    // check the mutually exclusive options.
    if ((Infinit::Parser->Test("Create") == elle::Status::True) &&
        (Infinit::Parser->Test("Destroy") == elle::Status::True) &&
        (Infinit::Parser->Test("Information") == elle::Status::True))
      {
        // display the usage.
        Infinit::Parser->Usage();

        escape("the create, destroy and information options are "
               "mutually exclusive");
      }

    // test the option.
    if (Infinit::Parser->Test("Create") == elle::Status::True)
      operation = Network::OperationCreate;

    // test the option.
    if (Infinit::Parser->Test("Destroy") == elle::Status::True)
      operation = Network::OperationDestroy;

    // test the option.
    if (Infinit::Parser->Test("Information") == elle::Status::True)
      operation = Network::OperationInformation;

    // trigger the operation.
    switch (operation)
      {
      case Network::OperationCreate:
        {
          elle::String          name;
          elle::String          string;
          hole::Model           model;
          elle::String          administrator;

          // retrieve the name.
          if (Infinit::Parser->Value("Name", name) == elle::Status::Error)
            escape("unable to retrieve the name value");

          // retrieve the model.
          if (Infinit::Parser->Value("Model", string) == elle::Status::Error)
            escape("unable to retrieve the model value");

          // build the model.
          if (model.Create(string) == elle::Status::Error)
            escape("unable to create the model");

          // retrieve the administrator.
          if (Infinit::Parser->Value("Administrator",
                                     administrator) == elle::Status::Error)
            escape("unable to retrieve the administrator value");

          // create the network.
          if (Network::Create(name, model, administrator) == elle::Status::Error)
            escape("unable to create the network");

          // display a message.
          std::cout << "The network has been created successfully!"
                    << std::endl;

          break;
        }
      case Network::OperationDestroy:
        {
          elle::String          name;

          // retrieve the name.
          if (Infinit::Parser->Value("Name", name) == elle::Status::Error)
            escape("unable to retrieve the name value");

          // destroy the network.
          if (Network::Destroy(name) == elle::Status::Error)
            escape("unable to destroy the network");

          // display a message.
          std::cout << "The network has been destroyed successfully!"
                    << std::endl;

          break;
        }
      case Network::OperationInformation:
        {
          elle::String          name;

          // retrieve the name.
          if (Infinit::Parser->Value("Name", name) == elle::Status::Error)
            escape("unable to retrieve the name value");

          // get information on the network.
          if (Network::Information(name) == elle::Status::Error)
            escape("unable to retrieve information on the network");

          break;
        }
      case Network::OperationUnknown:
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
