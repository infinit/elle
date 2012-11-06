#include <satellites/passport/Passport.hh>

#include <Infinit.hh>

#include <elle/cryptography/random.hh>
#include <elle/io/Console.hh>
#include <elle/utility/Parser.hh>
#include <elle/concurrency/Program.hh>

#include <etoile/Etoile.hh>

#include <hole/Hole.hh>

#include <lune/Lune.hh>
#include <elle/Authority.hh>
#include <elle/Passport.hh>

namespace satellite
{
//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method creates a new passport.
  ///
  elle::Status          Passport::Create()
  {
    //
    // test the arguments.
    //
    {
      // check if the authority exists.
      if (elle::Authority::exists(elle::io::Path(lune::Lune::Authority))
          == false)
        escape("unable to locate the authority file");
    }

    // Retrieve the authority.
    elle::String              prompt;
    elle::String              pass;

    // prompt the user for the passphrase.
    prompt = "Enter passphrase for the authority: ";

    if (elle::io::Console::Input(
          pass,
          prompt,
          elle::io::Console::OptionPassword) == elle::Status::Error)
      escape("unable to read the input");

    // load the authority.
    elle::Authority authority(elle::io::Path(lune::Lune::Authority));

    // decrypt the authority.
    if (authority.Decrypt(pass) == elle::Status::Error)
      escape("unable to decrypt the authority");

    //
    // create the passport.
    //
    {
      elle::Natural32 const id_length = 128;
      elle::Natural32 const buffer_size = 512;

      elle::String id(
        elle::cryptography::random::generate<elle::String>(id_length));
      elle::Buffer buffer(
        elle::cryptography::random::generate<elle::Buffer>(buffer_size));

      // create a label.
      hole::Label label(buffer);

      // create the passport.
      elle::Passport passport(label, id);

      // seal the passport.
      if (passport.Seal(authority) == elle::Status::Error)
        escape("unable to seal the passport");

      // store the passport.
      passport.store(elle::io::Path(lune::Lune::Passport));
    }

    return elle::Status::Ok;
  }

  ///
  /// this method destroys the existing passport.
  ///
  elle::Status          Passport::Destroy()
  {
    elle::Passport      passport;

    // does the passport exist.
    if (elle::Passport::exists(elle::io::Path(lune::Lune::Passport)) == true)
      elle::Passport::erase(elle::io::Path(lune::Lune::Passport));

    return elle::Status::Ok;
  }

  ///
  /// this method retrieves and displays information on the passport.
  ///
  elle::Status          Passport::Information()
  {
    elle::Passport      passport;

    //
    // test the arguments.
    //
    {
      // does the passport exist.
      if (elle::Passport::exists(elle::io::Path(lune::Lune::Passport)) == false)
        escape("this passport does not seem to exist");
    }

    //
    // retrieve the passport.
    //
    {
      // load the passport.
      passport.load(elle::io::Path(lune::Lune::Passport));

      // validate the passport.
      if (passport.Validate(Infinit::authority()) == elle::Status::Error)
        escape("unable to validate the passport");
    }

    //
    // display information.
    //
    {
      // dump the passport.
      if (passport.Dump() == elle::Status::Error)
        escape("unable to dump the passport");
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
    Passport::Operation operation;

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

    // initialize the Etoile library.
    if (etoile::Etoile::Initialize() == elle::Status::Error)
      escape("unable to initialize Etoile");

    // initialize the operation.
    operation = Passport::OperationUnknown;

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
          "create a new passport",
          elle::utility::Parser::KindNone) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Destroy",
          'd',
          "destroy",
          "destroy an existing passport",
          elle::utility::Parser::KindNone) == elle::Status::Error)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Information",
          'x',
          "information",
          "display information regarding a passport",
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
      operation = Passport::OperationCreate;

    // test the option.
    if (Infinit::Parser->Test("Destroy") == elle::Status::True)
      operation = Passport::OperationDestroy;

    // test the option.
    if (Infinit::Parser->Test("Information") == elle::Status::True)
      operation = Passport::OperationInformation;

    // trigger the operation.
    switch (operation)
      {
      case Passport::OperationCreate:
        {
          // create the passport.
          if (Passport::Create() == elle::Status::Error)
            escape("unable to create the passport");

          // display a message.
          std::cout << "The passport has been created successfully!"
                    << std::endl;

          break;
        }
      case Passport::OperationDestroy:
        {
          // destroy the passport.
          if (Passport::Destroy() == elle::Status::Error)
            escape("unable to destroy the passport");

          // display a message.
          std::cout << "The passport has been destroyed successfully!"
                    << std::endl;

          break;
        }
      case Passport::OperationInformation:
        {
          // get information on the passport.
          if (Passport::Information() == elle::Status::Error)
            escape("unable to retrieve information on the passport");

          break;
        }
      case Passport::OperationUnknown:
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

    // clean the Etoile.
    if (etoile::Etoile::Clean() == elle::Status::Error)
      escape("unable to clean Etoile");

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
