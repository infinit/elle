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
// updated       julien quintard   [fri mar  5 12:50:09 2010]
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
  Door			Agent::Socket;

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
      // create the door.
      if (Agent::Socket.Create() == StatusError)
	escape("unable to create the door");

      // connect the door.
      if (Agent::Socket.Connect(Agent::Line) == StatusError)
	escape("unable to connect to Etoile");
    }

    //
    // authenticate.
    //
    {
      Large		number;
      char*		hexadecimal;
      String		phrase;
      Code		code;

      // initialize and random generate a large.
      ::BN_init(&number);
      ::BN_rand(&number, 256, 0, 0);

      // retrieve the hexadecimal representation.
      hexadecimal = ::BN_bn2hex(&number);

      // build a string on this representation.
      phrase.assign(hexadecimal);

      std::cout << phrase << std::endl;

      // identify to etoile by passing the user's public key for challenging
      // along with the phrase.
      if (Agent::Socket.Call(Inputs< ::etoile::TagUserIdentify >(Agent::Pair.K,
								 phrase),
	                     Outputs< ::etoile::TagUserIdentify >(code)) ==
	  StatusError)
	escape("unable to identify to etoile");
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
// ---------- functions -------------------------------------------------------
//

  ///
  /// the main function.
  ///
  Status		Main(Natural32				argc,
			     Character*				argv[])
  {
    ::QCoreApplication	application((int&)argc, (char**)argv);
    String		name;

    enter();

    /// XXX \todo to improve with real argument parsing.
    {
      if (argc != 2)
	escape("[usage] ./agent {name}");

      name.assign(argv[1]);
    }

    // initialize the agent.
    if (Agent::Initialize(name) == StatusError)
      escape("unable to initialize the agent");

    // start the application event loop.
    application.exec();

    // clean the agent.
    if (Agent::Clean() == StatusError)
      escape("unable to clean the agent");

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
