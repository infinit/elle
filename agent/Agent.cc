//
// ---------- header ----------------------------------------------------------
//
// project       agent
//
// license       infinit
//
// file          /home/mycure/infinit/agent/Agent.cc
//
// created       julien quintard   [thu mar  4 17:51:46 2010]
// updated       julien quintard   [sat mar 20 16:25:09 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <agent/Agent.hh>

namespace agent
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this string contains the path to the user infinit configuration
  /// directory.
  ///
  const String		Agent::Path("/home/mycure/.infinit");

  ///
  /// this string represents the door's name to Etoile.
  ///
  const String		Agent::Line("etoile");

  ///
  /// the agent key pair though only the private key will be needed.
  ///
  KeyPair		Agent::Pair;

  ///
  /// the door to Etoile.
  ///
  Door			Agent::Link;

  ///
  /// the phrase used to connect applications to Etoile.
  ///
  String		Agent::Phrase;

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method initializes the agent, by fetching the user's key pair.
  ///
  Status		Agent::Initialize(const String&		name)
  {
    Archive		archive;
    Region		region;
    struct ::stat	stat;
    String		path = Agent::Path + "/keys/" + name + ".pair";
    Natural32		fd;

    enter();

    //
    // load the user's key pair.
    //
    {
      // get the file status.
      if (::stat(path.c_str(), &stat) == -1)
	escape(::strerror(errno));

      // prepare the region.
      if (region.Prepare(stat.st_size) == StatusError)
	escape("unable to prepare the region");

      // set the region size.
      region.size = stat.st_size;

      // open the file.
      if ((fd = ::open(path.c_str(), O_RDONLY)) == -1)
	escape(::strerror(errno));

      // read the file.
      if (::read(fd, region.contents, region.size) != region.size)
	{
	  ::close(fd);

	  escape("unable to read the key pair file");
	}

      // detach the contents from the region.
      if (region.Detach() == StatusError)
	escape("unable to detach the contents from the region");

      /// XXX \todo prompt the user for a passphrase and decrypt the content.

      // prepare the archive.
      if (archive.Prepare(region) == StatusError)
	escape("unable to prepare the region");

      // extract the key pair.
      if (archive.Extract(Agent::Pair) == StatusError)
	escape("unable to extract the key pair");
    }

    //
    // register the inward messages.
    //
    {
      // XXX
    }

    //
    // connect the agent to etoile.
    //
    {
      Function<const String>	monitor(&Agent::Monitor);

      // create the door.
      if (Agent::Link.Create() == StatusError)
	escape("unable to create the door");

      // monitor the socket.
      if (Agent::Link.Monitor(monitor) == StatusError)
	escape("unable to create the door");

      // connect the door.
      if (Agent::Link.Connect(Agent::Line) == StatusError)
	escape("unable to connect to Etoile");
    }

    //
    // authenticate.
    //
    {
      Code			code;
      Digest			digest;
      ::etoile::wall::Result	result;

      // identify to etoile by passing the user's public key for challenging
      // along with the phrase.
      if (Agent::Link.Call(
	    Inputs< ::etoile::TagWallIdentify >(Agent::Pair.K),
	    Outputs< ::etoile::TagWallChallenge >(code)) == StatusError)
	escape("unable to identify to etoile");

      // decrypt the code with the private key.
      if (Agent::Pair.k.Decrypt(code, Agent::Phrase) == StatusError)
	escape("unable to decrypt the code");

      // hash the phrase.
      if (OneWay::Hash(Agent::Phrase, digest) == StatusError)
	escape("unable to hash the phrase");

      // authenticate by sending the hash of the phrase.
      if (Agent::Link.Call(
	    Inputs< ::etoile::TagWallAuthenticate >(digest),
	    Outputs< ::etoile::TagWallAuthenticated >(result)) == StatusError)
	escape("unable to authenticate to etoile");

      // test the result.
      if (result == etoile::wall::Result::Error)
	printf("ERROR\n");
      //escape(result.report); // XXX method templates dans Report
    }

    leave();
  }

  ///
  /// this method cleans the agent.
  ///
  Status		Agent::Clean()
  {
    enter();

    leave();
  }

//
// ---------- callbacks -------------------------------------------------------
//

  ///
  /// this callback is triggered whenever an error occurs on the socket.
  ///
  Status		Agent::Monitor(const String&		error)
  {
    enter();

    printf("[XXX] Agent::Monitor()\n");

    // XXX
    //report(error);
    //Application::Exit();
    std::cerr << error << std::endl;

    leave();
  }

//
// ---------- functions -------------------------------------------------------
//

  ///
  /// the main function.
  ///
  Status		Main(Natural32				argc,
			     Character*				argv[])
  {
    String		name;

    enter();

    /// XXX \todo to improve with real argument parsing.
    {
      if (argc != 2)
	escape("[usage] ./agent {name}");

      name.assign(argv[1]);
    }

    // initialize the Elle library.
    if (Elle::Initialize() == StatusError)
      escape("unable to initialize the Elle library");

    // set up the application.
    if (Application::Setup(argc, argv) == StatusError)
      escape("unable to set up the application");

    // initialize the agent.
    if (Agent::Initialize(name) == StatusError)
      escape("unable to initialize the agent");

    // process the events.
    if (Application::Process() == StatusError)
      escape("unable to process the events");

    // clean the agent.
    if (Agent::Clean() == StatusError)
      escape("unable to clean the agent");

    // clean the Elle library.
    if (Elle::Clean() == StatusError)
      escape("unable to clean the Elle library");

    leave();
  }

}

//
// ---------- main ------------------------------------------------------------
//

///
/// this is the program entry point.
///
int			main(int				argc,
                             char**				argv)
{
  agent::Main(argc, argv);

  expose();

  return (0);
}
