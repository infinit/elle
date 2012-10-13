#include <elle/cryptography/PublicKey.hh>
#include <elle/cryptography/KeyPair.hh>
#include <elle/concurrency/Program.hh>

#include <elle/Elle.hh>
#include <elle/io/Console.hh>
#include <elle/io/Unique.hh>
#include <elle/utility/Parser.hh>
#include <elle/serialize/TupleSerializer.hxx>

#include <satellites/authority/Authority.hh>

namespace satellite
{

//
// ---------- definitions -----------------------------------------------------
//

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
    elle::cryptography::KeyPair       pair;
    lune::Authority     authority;

    // prompt the user for the passphrase.
    prompt = "Enter passphrase for the authority keypair: ";

    if (elle::io::Console::Input(
          pass,
          prompt,
          elle::io::Console::OptionPassword) == elle::Status::Error)
      escape("unable to read the input");

    // generate the authority key pair.
    if (pair.Generate(Authority::Length) == elle::Status::Error)
      escape("unable to generate the key pair");

    // create the authority with the generated key pair.
    if (authority.Create(pair) == elle::Status::Error)
      escape("unable to create the authority");

    // encrypt the authority.
    if (authority.Encrypt(pass) == elle::Status::Error)
      escape("unable to encrypt the authority");

    // store the authority.
    authority.store();

    return elle::Status::Ok;
  }

  ///
  /// this method destroys the existing authority.
  ///
  elle::Status          Authority::Destroy()
  {
    elle::String        prompt;
    elle::cryptography::KeyPair       pair;
    lune::Authority     authority;

    // erase the authority file.
    lune::Authority::erase();

    return elle::Status::Ok;
  }

  ///
  /// this method retrieves and displays information on the authority.
  ///
  elle::Status          Authority::Information()
  {
    elle::String        prompt;
    elle::String        pass;
    lune::Authority     authority;
    elle::io::Unique        unique;

    // check if the authority exists.
    if (lune::Authority::exists() == false)
      escape("unable to locate the authority file");

    // prompt the user for the passphrase.
    prompt = "Enter passphrase for the authority keypair: ";

    if (elle::io::Console::Input(
          pass,
          prompt,
          elle::io::Console::OptionPassword) == elle::Status::Error)
      escape("unable to read the input");

    // load the authority.
    authority.load();

    authority.Dump();

    // decrypt the authority.
    if (authority.Decrypt(pass) == elle::Status::Error)
      escape("unable to decrypt the authority");

    // dump the authority.
    if (authority.Dump() == elle::Status::Error)
      escape("unable to dump the authority");

    // retrive the public key's unique.
    if (authority.K.Save(unique) == elle::Status::Error)
      escape("unable to save the authority's public key");

    // dump the public key's unique so that it can be easily hard-coded in the
    // infinit software sources.
    std::cout << "[Unique] " << unique << std::endl;

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
    Authority::Operation        operation;

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
    operation = Authority::OperationUnknown;

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

    // register the options.
    if (Infinit::Parser->Register(
          "Create",
          'c',
          "create",
          "create the authority",
          elle::utility::Parser::KindNone) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Destroy",
          'd',
          "destroy",
          "destroy the existing authority",
          elle::utility::Parser::KindNone) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Information",
          'x',
          "information",
          "display information regarding the authority",
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
      operation = Authority::OperationCreate;

    // test the option.
    if (Infinit::Parser->Test("Destroy") == elle::Status::True)
      operation = Authority::OperationDestroy;

    // test the option.
    if (Infinit::Parser->Test("Information") == elle::Status::True)
      operation = Authority::OperationInformation;

    // trigger the operation.
    switch (operation)
      {
      case Authority::OperationCreate:
        {
          // create the authority.
          if (Authority::Create() == elle::Status::Error)
            escape("unable to create the authority");

          // display a message.
          std::cout << "The authority has been created successfully!"
                    << std::endl;

          break;
        }
      case Authority::OperationDestroy:
        {
          // destroy the authority.
          if (Authority::Destroy() == elle::Status::Error)
            escape("unable to destroy the authority");

          // display a message.
          std::cout << "The authority has been destroyed successfully!"
                    << std::endl;

          break;
        }
      case Authority::OperationInformation:
        {
          // get information on the authority.
          if (Authority::Information() == elle::Status::Error)
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
    Infinit::Parser = nullptr;

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

///
/// this is the program entry point.
///
int                     main(int                                argc,
                             char**                             argv)
{
  try
    {
      if (satellite::Main(argc, argv) == elle::Status::Error)
        return (1);
    }
  catch (std::exception& e)
    {
      std::cout << "The program has been terminated following "
                << "a fatal error (" << e.what() << ")." << std::endl;

      return (1);
    }

  return (0);
}
