#include <Infinit.hh>

#include <common/common.hh>

#include <elle/concurrency/Program.hh>
#include <elle/io/Piece.hh>
#include <elle/log.hh>
#include <elle/network/Interface.hh>
#include <elle/utility/Parser.hh>
#include <elle/cast.hh>
#include <elle/nat/Nat.hh>

#include <agent/Agent.hh>

#include <etoile/Etoile.hh>
#include <etoile/depot/Depot.hh>

#include <hole/Hole.hh>
#include <hole/implementations/slug/Implementation.hh>
#include <hole/storage/Directory.hh>
#include <horizon/Horizon.hh>

#include <lune/Descriptor.hh>
#include <lune/Lune.hh>

#include <plasma/meta/Client.hh>
#include <HoleFactory.hh>

#include <elle/CrashReporter.hh>

void
Infinit(elle::Natural32 argc, elle::Character* argv[])
{
  // set up the program.
  if (elle::concurrency::Program::Setup("Infinit") == elle::Status::Error)
    throw reactor::Exception(elle::concurrency::scheduler(),
                    "unable to set up the program");

  // allocate a new parser.
  Infinit::Parser = new elle::utility::Parser(argc, argv);

  // specify a program description.
  if (Infinit::Parser->Description(Infinit::Copyright) == elle::Status::Error)
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
        "User",
        'u',
        "user",
        "specifies the name of the user",
        elle::utility::Parser::KindRequired) == elle::Status::Error)
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

  // register the option.
  if (Infinit::Parser->Register(
        "Mountpoint",
        'm',
        "mountpoint",
        "specifies the mount point",
        elle::utility::Parser::KindRequired) == elle::Status::Error)
    throw reactor::Exception(elle::concurrency::scheduler(),
                    "unable to register the option");

  // parse.
  if (Infinit::Parser->Parse() == elle::Status::Error)
    throw reactor::Exception(elle::concurrency::scheduler(),
                    "unable to parse the command line");

  // test the option.
  if (Infinit::Parser->Test("Help") == true)
    {
      // display the usage.
      Infinit::Parser->Usage();

      // quit.
      return;
    }

  // retrieve the user name.
  if (Infinit::Parser->Value("User",
                             Infinit::User) == elle::Status::Error)
    {
      // display the usage.
      Infinit::Parser->Usage();

      throw reactor::Exception(elle::concurrency::scheduler(),
                      "unable to retrieve the user name");
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

  // Retrieve the mount point.
  if (Infinit::Parser->Test("Mountpoint") == true)
    {
      if (Infinit::Parser->Value("Mountpoint",
                                 Infinit::Mountpoint) == elle::Status::Error)
        throw reactor::Exception(elle::concurrency::scheduler(),
                                 "unable to retrieve the mountpoint");
    }
  else
    {
      // Nothing to do, keep the mounpoint empty.
      // XXX[to fix later though]
    }

  // initialize the Lune library.
  if (lune::Lune::Initialize() == elle::Status::Error)
    throw reactor::Exception(elle::concurrency::scheduler(),
                    "unable to initialize Lune");

  // initialize Infinit.
  if (Infinit::Initialize() == elle::Status::Error)
    throw reactor::Exception(elle::concurrency::scheduler(),
                    "unable to initialize Infinit");

  // initialize the Agent library.
  if (agent::Agent::Initialize() == elle::Status::Error)
    throw reactor::Exception(elle::concurrency::scheduler(),
                    "unable to initialize Agent");

  // // Create the NAT Manipulation class
  // elle::nat::NAT NAT(elle::concurrency::scheduler());
  // std::vector<std::pair<std::string, uint16_t>> public_addresses;


  // // By default, try to open a hole in the nat.
  // try
  //   {
  //     ELLE_DEBUG_SCOPE("start hole punching on %s:%d",
  //                      common::longinus::host(),
  //                      common::longinus::port());
  //     elle::nat::Hole pokey = NAT.punch(common::longinus::host(),
  //                                        common::longinus::port());

  //     public_addresses.push_back(pokey.public_endpoint());
  //   }
  // catch (elle::Exception &e)
  //   {
  //     ELLE_WARN("NAT punching error: %s", e.what());
  //   }

  nucleus::proton::Network network(Infinit::Network);
  elle::io::Path shelter_path(lune::Lune::Shelter);
  shelter_path.Complete(elle::io::Piece{"%USER%", Infinit::User},
                        elle::io::Piece{"%NETWORK%", Infinit::Network});
  hole::storage::Directory storage(network, shelter_path.string());

  elle::io::Path passport_path(lune::Lune::Passport);
  passport_path.Complete(elle::io::Piece{"%USER%", Infinit::User});
  elle::Passport passport;
  passport.load(passport_path);

  std::unique_ptr<hole::Hole> hole(
    infinit::hole_factory(storage, passport, Infinit::authority()));
  etoile::depot::hole(hole.get());
#ifdef INFINIT_HORIZON
  horizon::hole(hole.get());
#endif
  hole->join();

  // // FIXME
  // if (std::unique_ptr<hole::implementations::slug::Implementation> slug =
  //     elle::cast<hole::implementations::slug::Implementation>::runtime(hole))
  //   {
  //     lune::Descriptor descriptor(Infinit::User, Infinit::Network);
  //     plasma::meta::Client client(common::meta::host(), common::meta::port());
  //     try
  //       {
  //         std::vector<std::pair<std::string, uint16_t>> addresses;

  //         auto interfaces = elle::network::Interface::get_map(
  //           elle::network::Interface::Filter::only_up
  //           | elle::network::Interface::Filter::no_loopback
  //           );
  //         for (auto const& pair: interfaces)
  //           if (pair.second.ipv4_address.size() > 0 &&
  //               pair.second.mac_address.size() > 0)
  //             {
  //               addresses.emplace_back(pair.second.ipv4_address, slug->port());
  //               break;
  //             }
  //         if (addresses.size() == 0)
  //           {
  //             ELLE_ERR("Cannot find any valid ip address");
  //           }
  //         else
  //           {
  //             for (auto const &pair: addresses)
  //             {
  //               ELLE_LOG("Register instance address: %s:%d", pair.first,
  //                        pair.second);
  //             }

  //             client.token(agent::Agent::meta_token);
  //             // client.network_connect_device(descriptor.meta().id(),
  //             //                               passport.id(),
  //             //                               addresses,
  //             //                               public_addresses);
  //           }
  //       }
  //     catch (std::exception const& err)
  //       {
  //         ELLE_ERR("Cannot update device port: %s",
  //                  err.what()); // XXX[to improve]
  //       }
  //     hole.reset(slug.release());
  //   }

  // initialize the Etoile library.
  if (etoile::Etoile::Initialize() == elle::Status::Error)
    throw reactor::Exception(elle::concurrency::scheduler(),
                    "unable to initialize Etoile");

  // initialize the horizon.
  if (!Infinit::Mountpoint.empty())
#ifdef INFINIT_HORIZON
    if (horizon::Horizon::Initialize() == elle::Status::Error)
      throw reactor::Exception(elle::concurrency::scheduler(),
                               "unable to initialize the horizon");
#else
  throw reactor::Exception(elle::concurrency::scheduler(),
                           "horizon was disabled at compilation time "
                           "but a mountpoint was given on the command line");
#endif

  // launch the program.
  elle::concurrency::Program::Launch();

  // delete the parser.
  delete Infinit::Parser;
  Infinit::Parser = nullptr;

#ifdef INFINIT_HORIZON
  // clean the horizon.
  if (!Infinit::Mountpoint.empty())
    if (horizon::Horizon::Clean() == elle::Status::Error)
      throw reactor::Exception(elle::concurrency::scheduler(),
                               "unable to clean the horizon");
#endif

  // clean the Etoile library.
  if (etoile::Etoile::Clean() == elle::Status::Error)
    throw reactor::Exception(elle::concurrency::scheduler(),
                    "unable to clean Etoile");

  // clean the Agent library.
  if (agent::Agent::Clean() == elle::Status::Error)
    throw reactor::Exception(elle::concurrency::scheduler(),
                    "unable to clean Agent");

  hole->leave();
  delete hole.release();

  // clean Infinit.
  if (Infinit::Clean() == elle::Status::Error)
    throw reactor::Exception(elle::concurrency::scheduler(),
                    "unable to clean Infinit");

  // clean Lune
  if (lune::Lune::Clean() == elle::Status::Error)
    throw reactor::Exception(elle::concurrency::scheduler(),
                    "unable to clean Lune");
}

elle::Status
Main(elle::Natural32 argc, elle::Character* argv[])
{
  try
   {
      Infinit(argc, argv);
    }
  catch (elle::utility::ParserException const &e)
    {
      std::cerr << e.what() << std::endl;
      Infinit::Parser->Usage();
    }
  catch (std::exception const& e)
    {
      std::cerr << argv[0] << ": fatal error: " << e.what() << std::endl;
      if (reactor::Exception const* re =
          dynamic_cast<reactor::Exception const*>(&e))
        std::cerr << re->backtrace() << std::endl;

      elle::crash::report("8infinit", e.what(), reactor::Backtrace::current());
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
  reactor::VThread<elle::Status> main(sched,
                                      "Infinit main",
                                      [&argc, &argv] () -> elle::Status {
                                        return Main(argc, argv);
                                      });
  sched.run();

  return main.result() == elle::Status::Ok ? 0 : 1;
}
