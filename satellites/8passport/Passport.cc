//
// ---------- header ----------------------------------------------------------
//
// project       8passport
//
// license       infinit
//
// author        julien quintard   [thu aug 11 15:16:04 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <applications/8passport/Passport.hh>

namespace application
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const elle::Character         Component[] = "8passport";

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method creates a new passport.
  ///
  elle::Status          Passport::Create()
  {
    lune::Authority     authority;
    lune::Passport      passport;

    enter();

    //
    // test the arguments.
    //
    {
      // check if the authority exists.
      if (authority.Exist() == elle::StatusFalse)
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
            elle::Console::OptionPassword) == elle::StatusError)
        escape("unable to read the input");

      // load the authority.
      if (authority.Load() == elle::StatusError)
        escape("unable to load the authority");

      // decrypt the authority.
      if (authority.Decrypt(pass) == elle::StatusError)
        escape("unable to decrypt the authority");
    }

    //
    // create the passport.
    //
    {
      hole::Label               label;
      elle::Region              region;

      // generate a random region.
      if (elle::Random::Generate(region) == elle::StatusError)
        escape("unable to generate a random region");

      // create a label.
      if (label.Create(region) == elle::StatusError)
        escape("unable to create a label");

      // create the passport.
      if (passport.Create(label) == elle::StatusError)
        escape("unable to create the passport");

      // seal the passport.
      if (passport.Seal(authority) == elle::StatusError)
        escape("unable to seal the passport");

      // store the passport.
      if (passport.Store() == elle::StatusError)
        escape("unable to store the passport");
    }

    leave();
  }

  ///
  /// this method destroys the existing passport.
  ///
  elle::Status          Passport::Destroy()
  {
    lune::Passport      passport;

    enter();

    // does the passport exist.
    if (passport.Exist() == elle::StatusTrue)
      {
        // remove the passport.
        if (passport.Erase() == elle::StatusError)
          escape("unable to erase the passport");
      }

    leave();
  }

  ///
  /// this method retrieves and displays information on the passport.
  ///
  elle::Status          Passport::Information()
  {
    lune::Passport      passport;

    enter();

    //
    // test the arguments.
    //
    {
      // does the passport exist.
      if (passport.Exist() == elle::StatusFalse)
        escape("this passport does not seem to exist");
    }

    //
    // retrieve the passport.
    //
    {
      // load the passport.
      if (passport.Load() == elle::StatusError)
        escape("unable to load the passport");

      // validate the passport.
      if (passport.Validate(Infinit::Authority) == elle::StatusError)
        escape("unable to validate the passport");
    }

    //
    // display information.
    //
    {
      // dump the passport.
      if (passport.Dump() == elle::StatusError)
        escape("unable to dump the passport");
    }

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
    Passport::Operation operation;

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

    // initialize the Etoile library.
    if (etoile::Etoile::Initialize() == elle::StatusError)
      escape("unable to initialize Etoile");

    // initialize the operation.
    operation = Passport::OperationUnknown;

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
          "create a new passport",
          elle::Parser::KindNone) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Destroy",
          'd',
          "destroy",
          "destroy an existing passport",
          elle::Parser::KindNone) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Information",
          'x',
          "information",
          "display information regarding a passport",
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
      operation = Passport::OperationCreate;

    // test the option.
    if (Infinit::Parser->Test("Destroy") == elle::StatusTrue)
      operation = Passport::OperationDestroy;

    // test the option.
    if (Infinit::Parser->Test("Information") == elle::StatusTrue)
      operation = Passport::OperationInformation;

    // trigger the operation.
    switch (operation)
      {
      case Passport::OperationCreate:
        {
          // create the passport.
          if (Passport::Create() == elle::StatusError)
            escape("unable to create the passport");

          // display a message.
          std::cout << "The passport has been created successfully!"
                    << std::endl;

          break;
        }
      case Passport::OperationDestroy:
        {
          // destroy the passport.
          if (Passport::Destroy() == elle::StatusError)
            escape("unable to destroy the passport");

          // display a message.
          std::cout << "The passport has been destroyed successfully!"
                    << std::endl;

          break;
        }
      case Passport::OperationInformation:
        {
          // get information on the passport.
          if (Passport::Information() == elle::StatusError)
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
      if (application::Main(argc, argv) == elle::StatusError)
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
