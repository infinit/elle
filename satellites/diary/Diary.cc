//
// ---------- header ----------------------------------------------------------
//
// project       diary
//
// license       infinit
//
// author        julien quintard   [mon jun 27 22:38:51 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <satellites/diary/Diary.hh>

namespace satellite
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const elle::Character         Component[] = "8diary";

//
// ---------- functions -------------------------------------------------------
//

  ///
  /// the main function.
  ///
  elle::Status          Main(elle::Natural32                    argc,
                             elle::Character*                   argv[])
  {
    Diary::Operation    operation;

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
    operation = Diary::OperationUnknown;

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
          "Record",
          'c',
          "record",
          "activate the event recording",
          elle::Parser::KindRequired) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Replay",
          'y',
          "replay",
          "activate the event replaying",
          elle::Parser::KindRequired) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Dump",
          'd',
          "dump",
          "activate the dumping of the given diary",
          elle::Parser::KindRequired) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Mountpoint",
          'm',
          "mounpoint",
          "specify the path to the file system mounpoint",
          elle::Parser::KindRequired) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "Mirror",
          'i',
          "mirror",
          "specify the path to the directory which must be mirrored through "
          "the mounpoint",
          elle::Parser::KindRequired) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "From",
          'f',
          "from",
          "specify the number of the first operation to be triggered from "
          "the diary",
          elle::Parser::KindRequired) == elle::StatusError)
      escape("unable to register the option");

    // register the option.
    if (Infinit::Parser->Register(
          "Network",
          'n',
          "network",
          "specifies the name of the network",
          elle::Parser::KindRequired) == elle::StatusError)
      escape("unable to register the option");

    // register the options.
    if (Infinit::Parser->Register(
          "To",
          't',
          "to",
          "specify the number of the last operation to be triggered from "
          "the diary",
          elle::Parser::KindRequired) == elle::StatusError)
      escape("unable to register the option");

    // add an example.
    if (Infinit::Parser->Example(
          "-c test.dia -m ~/local/mnt/test/ -i /tmp/test") ==
        elle::StatusError)
      escape("unable to set an example");

    // add an example.
    if (Infinit::Parser->Example(
          "-y test.dia -n mynetwork") ==
        elle::StatusError)
      escape("unable to set an example");

    // parse.
    if (Infinit::Parser->Parse() == elle::StatusError)
      escape("unable to parse the command line");

    // test the option.
    if (Infinit::Parser->Test("Help") == elle::StatusTrue)
      {
        // display the usage.
        Infinit::Parser->Usage();

        // quit.
        return elle::StatusOk;
      }

    // initialize the Etoile library.
    if (etoile::Etoile::Initialize() == elle::StatusError)
      escape("unable to initialize Etoile");

    // check the mutually exclusive options.
    if ((Infinit::Parser->Test("Record") == elle::StatusTrue) &&
        (Infinit::Parser->Test("Replay") == elle::StatusTrue) &&
        (Infinit::Parser->Test("Dump") == elle::StatusTrue))
      escape("the record and replay options are mutually exclusive");

    // test the option.
    if (Infinit::Parser->Test("Record") == elle::StatusTrue)
      operation = Diary::OperationRecord;

    // test the option.
    if (Infinit::Parser->Test("Replay") == elle::StatusTrue)
      operation = Diary::OperationReplay;

    // test the option.
    if (Infinit::Parser->Test("Dump") == elle::StatusTrue)
      operation = Diary::OperationDump;

    // trigger a command.
    switch (operation)
      {
      case Diary::OperationRecord:
        {
          elle::String          mountpoint;
          elle::String          mirror;
          elle::String          string;
          elle::Path            path;

          // retrieve the string-based path.
          if (Infinit::Parser->Value("Record",
                                     string) == elle::StatusError)
            escape("unable to retrieve the path value");

          // create the path.
          if (path.Create(string) == elle::StatusError)
            escape("unable to create the path");

          // retrieve the mountpoint.
          if (Infinit::Parser->Value("Mountpoint",
                                     mountpoint) == elle::StatusError)
            escape("unable to retrieve the mountpoint value");

          // retrieve the mirror.
          if (Infinit::Parser->Value("Mirror",
                                     mirror) == elle::StatusError)
            escape("unable to retrieve the mirror value");

#if defined(INFINIT_LINUX) || defined(INFINIT_MACOSX)
          {
            unix::Memoirs       memoirs;

            // initialize the memoirs.
            if (memoirs.Initialize(mountpoint, mirror) == elle::StatusError)
              escape("unable to initialize the memoirs");

            // launch the program.
            if (elle::Program::Launch() == elle::StatusError)
              escape("an error occured while processing events");

            // clean the memoirs.
            if (memoirs.Clean() == elle::StatusError)
              escape("unable to clean the memoirs");

            // store the memoirs.
            if (memoirs.Store(path) == elle::StatusError)
              escape("unable to store the memoirs");
          }
#elif defined(INFINIT_WINDOWS)
          {
            // XXX todo: windows
          }
#else
# error "unsupported platform"
#endif

          // display a message.
          std::cout << "The sequence of file system operations have been "
                    << "successfully recorded in '" << path.str() << "'!"
                    << std::endl;

          break;
        }
      case Diary::OperationReplay:
        {
          elle::String          string;
          elle::Path            path;
          elle::Natural32       from;
          elle::Natural32       to;

          // retrieve the string-based path.
          if (Infinit::Parser->Value("Replay",
                                     string) == elle::StatusError)
            escape("unable to retrieve the path value");

          // create the path.
          if (path.Create(string) == elle::StatusError)
            escape("unable to create the path");

          // initialize the indexes.
          from = 0;
          to = elle::Variable::Maximum(to);

          // retrieve the from.
          if ((Infinit::Parser->Test("From") == elle::StatusTrue) &&
              (Infinit::Parser->Value(
                 "From",
                 from) == elle::StatusError))
              escape("unable to retrieve the from value");

          // retrieve the to.
          if ((Infinit::Parser->Test("To") == elle::StatusTrue) &&
              (Infinit::Parser->Value(
                 "To",
                 to) == elle::StatusError))
            escape("unable to retrieve the to value");

          // retrieve the network name.
          if (Infinit::Parser->Value("Network",
                                     Infinit::Network) == elle::StatusError)
            escape("unable to retrieve the network name");

          // initialize the Hole library.
          if (hole::Hole::Initialize() == elle::StatusError)
            escape("unable to initialize Hole");

          // initialize the Agent library.
          if (agent::Agent::Initialize() == elle::StatusError)
            escape("unable to initialize Agent");

#if defined(INFINIT_LINUX) || defined(INFINIT_MACOSX)
          {
            unix::Memoirs       memoirs;

            // load the memoirs.
            if (memoirs.Load(path) == elle::StatusError)
              escape("unable to load the memoirs");

            // initialize the memoirs.
            if (memoirs.Initialize(from, to) == elle::StatusError)
              escape("unable to initialize the memoirs");

            // launch the program.
            if (elle::Program::Launch() == elle::StatusError)
              escape("an error occured while processing events");

            // clean the memoirs.
            if (memoirs.Clean() == elle::StatusError)
              escape("unable to clean the memoirs");
          }
#elif defined(INFINIT_WINDOWS)
          {
            // XXX todo: windows
          }
#else
# error "unsupported platform"
#endif

          // clean the Agent library.
          if (agent::Agent::Clean() == elle::StatusError)
            escape("unable to clean Agent");

          // clean Hole.
          if (hole::Hole::Clean() == elle::StatusError)
            escape("unable to clean Hole");

          break;
        }
      case Diary::OperationDump:
        {
          elle::String          string;
          elle::Path            path;

          // retrieve the string-based path.
          if (Infinit::Parser->Value("Dump",
                                     string) == elle::StatusError)
            escape("unable to retrieve the path value");

          // create the path.
          if (path.Create(string) == elle::StatusError)
            escape("unable to create the path");

#if defined(INFINIT_LINUX) || defined(INFINIT_MACOSX)
          {
            unix::Memoirs       memoirs;

            // load the memoirs.
            if (memoirs.Load(path) == elle::StatusError)
              escape("unable to load the memoirs");

            // dump the memoirs.
            if (memoirs.Dump() == elle::StatusError)
              escape("unable to dump the memoirs");
          }
#elif defined(INFINIT_WINDOWS)
          {
            // XXX todo: windows
          }
#else
# error "unsupported platform"
#endif

          break;
        }
      case Diary::OperationUnknown:
      default:
        {
          // display the usage.
          Infinit::Parser->Usage();

          escape("please specify an operation to perform");
        }
      }

    // delete the parser.
    delete Infinit::Parser;

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

    return elle::StatusOk;
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
