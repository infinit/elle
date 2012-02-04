//
// ---------- header ----------------------------------------------------------
//
// project       authority
//
// license       infinit
//
// author        julien quintard   [thu mar  4 17:51:46 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <satellites/authority/Authority.hh>

namespace satellite
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const elle::Character         Component[] = "8authority";

  ///
  /// this value defines the authority key pair length.
  ///
  /// the length is kept high in order to make attacks more difficult.
  ///
  const elle::Natural32         Authority::Length = 4096;

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method creates a new authority.
  ///
  elle::Status          Authority::Create()
  {
    elle::String        prompt;
    elle::String        pass;
    elle::KeyPair       pair;
    lune::Authority     authority;

    enter();

    // prompt the user for the passphrase.
    prompt = "Enter passphrase for the authority keypair: ";

    if (elle::Console::Input(
          pass,
          prompt,
          elle::Console::OptionPassword) == elle::StatusError)
      escape("unable to read the input");

    // generate the authority key pair.
    if (pair.Generate(Authority::Length) == elle::StatusError)
      escape("unable to generate the key pair");

    // create the authority with the generated key pair.
    if (authority.Create(pair) == elle::StatusError)
      escape("unable to create the authority");

    // encrypt the authority.
    if (authority.Encrypt(pass) == elle::StatusError)
      escape("unable to encrypt the authority");

    // store the authority.
    if (authority.Store() == elle::StatusError)
      escape("unable to store the authority");

    leave();
  }

  ///
  /// this method destroys the existing authority.
  ///
  elle::Status          Authority::Destroy()
  {
    elle::String        prompt;
    elle::KeyPair       pair;
    lune::Authority     authority;

    enter();

    // erase the authority file.
    if (authority.Erase() == elle::StatusError)
      escape("unable to erase the authority");

    leave();
  }

  ///
  /// this method retrieves and displays information on the authority.
  ///
  elle::Status          Authority::Information()
  {
    elle::String        prompt;
    elle::String        pass;
    lune::Authority     authority;
    elle::Unique        unique;

    enter();

    // check if the authority exists.
    if (authority.Exist() == elle::StatusFalse)
      escape("unable to locate the authority file");

    // prompt the user for the passphrase.
    prompt = "Enter passphrase for the authority keypair: ";

    if (elle::Console::Input(
          pass,
          prompt,
          elle::Console::OptionPassword) == elle::StatusError)
      escape("unable to read the input");

    // load the authority.
    if (authority.Load() == elle::StatusError)
      escape("unable to load the authority file");

    // decrypt the authority.
    if (authority.Decrypt(pass) == elle::StatusError)
      escape("unable to decrypt the authority");

    // dump the authority.
    if (authority.Dump() == elle::StatusError)
      escape("unable to dump the authority");

    // retrive the public key's unique.
    if (authority.K.Save(unique) == elle::StatusError)
      escape("unable to save the authority's public key");

    // dump the public key's unique so that it can be easily hard-coded in the
    // infinit software sources.
    std::cout << "[Unique] " << unique << std::endl;

    leave();
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
    Authority::Operation        operation;

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
    operation = Authority::OperationUnknown;

    // initialize the Etoile library.
    if (etoile::Etoile::Initialize() == elle::StatusError)
      escape("unable to initialize Etoile");

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

    // register the options.
    if (Infinit::Parser->Register(
          "Create",
          'c',
          "create",
          "create the authority",
          elle::Parser::KindNone) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Destroy",
          'd',
          "destroy",
          "destroy the existing authority",
          elle::Parser::KindNone) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Information",
          'x',
          "information",
          "display information regarding the authority",
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

    // check the mutually exclusive options.
    if ((Infinit::Parser->Test("Create") == elle::StatusTrue) &&
        (Infinit::Parser->Test("Destroy") == elle::StatusTrue) &&
        (Infinit::Parser->Test("Information") == elle::StatusTrue))
      {
        // display the usage.
        Infinit::Parser->Usage();

        escape("the create, destroy and information options are "
               "mutually exclusive");
      }

    // test the option.
    if (Infinit::Parser->Test("Create") == elle::StatusTrue)
      operation = Authority::OperationCreate;

    // test the option.
    if (Infinit::Parser->Test("Destroy") == elle::StatusTrue)
      operation = Authority::OperationDestroy;

    // test the option.
    if (Infinit::Parser->Test("Information") == elle::StatusTrue)
      operation = Authority::OperationInformation;

    // trigger the operation.
    switch (operation)
      {
      case Authority::OperationCreate:
        {
          // create the authority.
          if (Authority::Create() == elle::StatusError)
            escape("unable to create the authority");

          // display a message.
          std::cout << "The authority has been created successfully!"
                    << std::endl;

          break;
        }
      case Authority::OperationDestroy:
        {
          // destroy the authority.
          if (Authority::Destroy() == elle::StatusError)
            escape("unable to destroy the authority");

          // display a message.
          std::cout << "The authority has been destroyed successfully!"
                    << std::endl;

          break;
        }
      case Authority::OperationInformation:
        {
          // get information on the authority.
          if (Authority::Information() == elle::StatusError)
            escape("unable to retrieve information on the authority");

          break;
        }
      case Authority::OperationUnknown:
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

    // clean the Etoile.
    if (etoile::Etoile::Clean() == elle::StatusError)
      escape("unable to clean Etoile");

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
