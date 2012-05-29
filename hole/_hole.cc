#include <Infinit.hh>

#include <elle/types.hh>
#include <elle/Elle.hh>
#include <elle/utility/Parser.hh>
#include <elle/concurrency/Program.hh>

#include <nucleus/Nucleus.hh>
#include <lune/Lune.hh>
#include <hole/Hole.hh>
#include <lune/Lune.hh>
#include <nucleus/Nucleus.hh>

namespace hole
{
  void
  Hole(elle::Natural32 argc, elle::Character* argv[])
  {
    // XXX Infinit::Parser is not deleted in case of error

    // initialize the Elle library.
    if (elle::Elle::Initialize() == elle::StatusError)
      throw std::runtime_error("unable to initialize Elle");

    // initialize the nucleus library.
    if (nucleus::Nucleus::Initialize() == elle::StatusError)
      throw std::runtime_error("unable to initialize Nucleus");

    // initialize the Lune library.
    if (lune::Lune::Initialize() == elle::StatusError)
      throw std::runtime_error("unable to initialize Lune");

    // initialize Infinit.
    if (Infinit::Initialize() == elle::StatusError)
      throw std::runtime_error("unable to initialize Infinit");

    // set up the program.
    if (elle::Program::Setup() == elle::StatusError)
      throw std::runtime_error("unable to set up the program");

    // allocate a new parser.
    Infinit::Parser = new elle::Parser(argc, argv);

    // specify a program description.
    if (Infinit::Parser->Description(Infinit::Version +
                                     " "+
                                     "Copyright (c) 2008, 2009, 2010, 2011, "
                                     "Julien Quintard, All rights "
                                     "reserved.\n") == elle::StatusError)
      throw std::runtime_error("unable to set the description");

    // register the options.
    if (Infinit::Parser->Register(
          "Help",
          'h',
          "help",
          "display the help",
          elle::Parser::KindNone) == elle::StatusError)
      throw std::runtime_error("unable to register the option");

    // register the option.
    if (Infinit::Parser->Register(
          "Network",
          'n',
          "network",
          "specifies the name of the network",
          elle::Parser::KindRequired) == elle::StatusError)
      throw std::runtime_error("unable to register the option");

    // parse.
    if (Infinit::Parser->Parse() == elle::StatusError)
      throw std::runtime_error("unable to parse the command line");

    // test the option.
    if (Infinit::Parser->Test("Help") == elle::Status::True)
      {
        Infinit::Parser->Usage();
        return;
      }

    // retrieve the network name.
    if (Infinit::Parser->Value("Network",
                               Infinit::Network) == elle::Status::Error)
      {
        // display the usage.
        Infinit::Parser->Usage();

        throw std::runtime_error("unable to retrieve the network name");
      }

    // initialize the Hole library.
    hole::Hole::Initialize();

    // launch the program.
    elle::Program::Launch();

    // delete the parser.
    delete Infinit::Parser;
    Infinit::Parser = nullptr;

    // clean Hole.
    if (hole::Hole::Clean() == elle::StatusError)
      throw std::runtime_error("unable to clean Hole");

    // clean Infinit.
    if (Infinit::Clean() == elle::StatusError)
      throw std::runtime_error("unable to clean Infinit");

    // clean Lune
    if (lune::Lune::Clean() == elle::StatusError)
      throw std::runtime_error("unable to clean Lune");

    // clean the nucleus library.
    if (nucleus::Nucleus::Clean() == elle::StatusError)
      throw std::runtime_error("unable to clean Nucleus");

    // clean Elle.
    if (elle::Elle::Clean() == elle::StatusError)
      throw std::runtime_error("unable to clean Elle");
  }
}

elle::Status
Main(elle::Natural32 argc, elle::Character* argv[])
{
  try
    {
      hole::Hole(argc, argv);
    }
  catch (std::runtime_error& e)
    {
      std::cerr << argv[0] << ": fatal error: " << e.what() << std::endl;
      elle::concurrency::scheduler().terminate();
      return elle::StatusError;
    }
  elle::concurrency::scheduler().terminate();
  return elle::StatusOk;
}

int
main(int argc, char* argv[])
{
  reactor::Scheduler& sched = elle::concurrency::scheduler();
  reactor::VThread<elle::Status> main(sched, "Hole main",
                                      boost::bind(&Main, argc, argv));
  sched.run();
  return main.result() == elle::StatusOk ? 0 : 1;
}
