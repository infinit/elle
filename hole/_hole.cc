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
    if (elle::Elle::Initialize() == elle::Status::Error)
      throw reactor::Exception(elle::concurrency::scheduler(),
                      "unable to initialize Elle");

    // initialize the nucleus library.
    if (nucleus::Nucleus::Initialize() == elle::Status::Error)
      throw reactor::Exception(elle::concurrency::scheduler(),
                      "unable to initialize Nucleus");

    // initialize the Lune library.
    if (lune::Lune::Initialize() == elle::Status::Error)
      throw reactor::Exception(elle::concurrency::scheduler(),
                      "unable to initialize Lune");

    // initialize Infinit.
    if (Infinit::Initialize() == elle::Status::Error)
      throw reactor::Exception(elle::concurrency::scheduler(),
                      "unable to initialize Infinit");

    // set up the program.
    if (elle::concurrency::Program::Setup() == elle::Status::Error)
      throw reactor::Exception(elle::concurrency::scheduler(),
                      "unable to set up the program");

    // allocate a new parser.
    Infinit::Parser = new elle::utility::Parser(argc, argv);

    // specify a program description.
    if (Infinit::Parser->Description(elle::sprint(Infinit::version) +
                                     " " +
                                     "Copyright (c) 2008, 2009, 2010, 2011, "
                                     "Julien Quintard, All rights "
                                     "reserved.\n") == elle::Status::Error)
      throw reactor::Exception(elle::concurrency::scheduler(),
                      "unable to set the description");

    // register the options.
    if (Infinit::Parser->Register(
          "Help",
          'h',
          "help",
          "display the help",
          elle::utility::Parser::KindNone) == elle::Status::Error)
      throw reactor::Exception(elle::concurrency::scheduler(),
                      "unable to register the option");

    // register the option.
    if (Infinit::Parser->Register(
          "Network",
          'n',
          "network",
          "specifies the name of the network",
          elle::utility::Parser::KindRequired) == elle::Status::Error)
      throw reactor::Exception(elle::concurrency::scheduler(),
                      "unable to register the option");

    // parse.
    if (Infinit::Parser->Parse() == elle::Status::Error)
      throw reactor::Exception(elle::concurrency::scheduler(),
                      "unable to parse the command line");

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

        throw reactor::Exception(elle::concurrency::scheduler(),
                        "unable to retrieve the network name");
      }

    // initialize the Hole library.
    hole::Hole::Initialize();
    hole::Hole::join();

    // launch the program.
    elle::concurrency::Program::Launch();

    // delete the parser.
    delete Infinit::Parser;
    Infinit::Parser = nullptr;

    hole::Hole::leave();

    // clean Hole.
    if (hole::Hole::Clean() == elle::Status::Error)
      throw reactor::Exception(elle::concurrency::scheduler(),
                      "unable to clean Hole");

    // clean Infinit.
    if (Infinit::Clean() == elle::Status::Error)
      throw reactor::Exception(elle::concurrency::scheduler(),
                      "unable to clean Infinit");

    // clean Lune
    if (lune::Lune::Clean() == elle::Status::Error)
      throw reactor::Exception(elle::concurrency::scheduler(),
                      "unable to clean Lune");

    // clean the nucleus library.
    if (nucleus::Nucleus::Clean() == elle::Status::Error)
      throw reactor::Exception(elle::concurrency::scheduler(),
                      "unable to clean Nucleus");

    // clean Elle.
    if (elle::Elle::Clean() == elle::Status::Error)
      throw reactor::Exception(elle::concurrency::scheduler(),
                      "unable to clean Elle");
  }
}

elle::Status
Main(elle::Natural32 argc, elle::Character* argv[])
{
  try
    {
      hole::Hole(argc, argv);
    }
  catch (std::exception const& e)
    {
      std::cerr << argv[0] << ": fatal error: " << e.what() << std::endl;
      if (reactor::Exception const* re
          = dynamic_cast<reactor::Exception const*>(&e))
        std::cerr << re->backtrace() << std::endl;
      elle::concurrency::scheduler().terminate();
      return elle::Status::Error;
    }
  elle::concurrency::scheduler().terminate();
  return elle::Status::Ok;
}

int
main(int argc, char* argv[])
{
  reactor::Scheduler& sched = elle::concurrency::scheduler();
  reactor::VThread<elle::Status> main(sched, "Hole main",
                                      boost::bind(&Main, argc, argv));
  sched.run();
  return main.result() == elle::Status::Ok ? 0 : 1;
}
