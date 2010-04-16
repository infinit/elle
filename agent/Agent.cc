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
// updated       julien quintard   [thu apr 15 13:41:51 2010]
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
  /// this value defines the component's name.
  ///
  const Character		Component[] = "agent";

  ///
  /// this string contains the path to the user infinit configuration
  /// directory.
  ///
  const String			Agent::Path = System::Path::Home + "/.infinit";

  ///
  /// this string represents the door's name to Etoile.
  ///
  const String			Agent::Line("etoile");

  ///
  /// the agent key pair though only the private key will be needed.
  ///
  KeyPair			Agent::Pair;

  ///
  /// the door to Etoile.
  ///
  Door				Agent::Channel;

  ///
  /// the phrase used to connect applications to Etoile.
  ///
  String			Agent::Phrase;

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
    Integer32		fd;
    String		prompt;
    String		passphrase;
    SecretKey		key;
    Cipher		cipher;

    enter();

    //
    // load the user's key pair.
    //
    {
      // get the file status.
      if (::stat(path.c_str(), &stat) == -1)
	escape("unable to access the user's key pair");

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

      // close the file.
      ::close(fd);

      // detach the contents from the region.
      if (region.Detach() == StatusError)
	escape("unable to detach the contents from the region");

      // prepare the archive with the region.
      if (archive.Prepare(region) == StatusError)
	escape("unable to prepare the archive");

      // extract the cipher from the ciphered archive.
      if (archive.Extract(cipher) == StatusError)
	escape("unable to extract the cipher");

      // prompt the user for the passphrase.
      prompt = "Enter passphrase for keypair '" + path + "': ";
      passphrase = String(::getpass(prompt.c_str()));

      // create a secret key with this passphrase.
      if (key.Create(passphrase) == StatusError)
	escape("unable to create the secret key");

      // decrypt the keypair file content with the secret key.
      if (key.Decrypt(cipher, Agent::Pair) == StatusError)
	escape("unable to decrypt the keypair");
    }

    //
    // dump the user's identity.
    //
    {
      // dump the name.
      std::cout << "[Name] " << name << std::endl;

      // dump the public key.
      if (Agent::Pair.K.Dump() == StatusError)
	escape("unable to dump the user's public key");
    }

    //
    // register the inward messages.
    //
    {
      Callback<const Report>	error(&Agent::Error);
      Callback<const Code>	decrypt(&Agent::Decrypt);
      Callback<const Plain>	sign(&Agent::Sign);

      // register the error message.
      if (Network::Register<TagError>(error) == StatusError)
        escape("unable to register the error callback");

      // register the decrypt message.
      if (Network::Register<TagDecrypt>(decrypt) == StatusError)
        escape("unable to register the decrypt callback");

      // register the sign message.
      if (Network::Register<TagSign>(sign) == StatusError)
        escape("unable to register the sign callback");
    }

    //
    // trigger the Start() callback in a fiber so that it can block.
    //
    {
      Entrance<>		start(&Agent::Start);
      Closure<>			closure(start);

      // spawn a fiber.
      if (Fiber::Spawn(closure) == StatusError)
	escape("unable to spawn a fiber");
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

  ///
  /// this method authenticate the agent to Etoile.
  ///
  Status		Agent::Authenticate()
  {
    enter();

    //
    // connect the agent to etoile.
    //
    {
      Callback<const String>	error(&Agent::Error);

      // create the door.
      if (Agent::Channel.Create() == StatusError)
	escape("unable to create the door");

      // monitor the socket.
      if (Agent::Channel.Monitor(error) == StatusError)
	escape("unable to create the door");

      // connect the door.
      if (Agent::Channel.Connect(Agent::Line) == StatusError)
	escape("unable to connect to Etoile");
    }

    //
    // authenticate.
    //
    {
      Code			code;
      Digest			digest;

      // identify to etoile by passing the user's public key for challenging
      // along with the phrase.
      if (Agent::Channel.Call(
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
      if (Agent::Channel.Call(
	    Inputs< ::etoile::TagWallAuthenticate >(digest),
	    Outputs< ::etoile::TagOk >()) == StatusError)
	escape("unable to authenticate to etoile");
    }

    //
    // store the passphrase in the infinit configuration so that
    // applications can connect to etoile on behalf of the agent's user.
    //
    {
      String		path = Agent::Path + "/phrase";
      Archive		archive;
      Integer32		fd;

      // create an archive.
      if (archive.Create() == StatusError)
	escape("unable to create an archive");

      // serialize the user public key and phrase.
      if (archive.Serialize(Agent::Pair.K, Agent::Phrase) == StatusError)
	escape("unable to serialize the public key and phrase");

      // create the file or overwrite it.
      if ((fd = ::open(path.c_str(),
		       O_WRONLY | O_TRUNC | O_CREAT,
		       0600)) == -1)
	escape(::strerror(errno));

      // write the file.
      if (::write(fd, archive.contents, archive.size) != archive.size)
	{
	  ::close(fd);

	  escape("unable to write the phrase file");
	}

      // close the file.
      ::close(fd);
    }

    leave();
  }

//
// ---------- callbacks -------------------------------------------------------
//

  ///
  /// this callback is triggered as soon as the agent has been initialized.
  ///
  Status		Agent::Start()
  {
    enter();

    // authenticate the agent.
    if (Agent::Authenticate() == StatusError)
      Program::Exit(StatusError);

    leave();
  }

  ///
  /// this callback is triggered whenever Etoile needs to decrypt
  /// a code with the agent's private key.
  ///
  Status		Agent::Decrypt(const Code&		code)
  {
    Clear		clear;

    enter();

    // perform the cryptographic operation.
    if (Agent::Pair.k.Decrypt(code, clear) == StatusError)
      escape("unable to perform the decryption");

    // reply to the caller.
    if (Agent::Channel.Reply(Inputs<TagDecrypted>(clear)) == StatusError)
      escape("unable to reply to the caller");

    leave();
  }

  ///
  /// this callback is triggered whenever Etoile needs to perform a signature.
  ///
  Status		Agent::Sign(const Plain&		plain)
  {
    Signature		signature;

    enter();

    // perform the cryptographic operation.
    if (Agent::Pair.k.Sign(plain, signature) == StatusError)
      escape("unable to perform the signature");

    // reply to the caller.
    if (Agent::Channel.Reply(Inputs<TagSigned>(signature)) == StatusError)
      escape("unable to reply to the caller");

    leave();
  }

  ///
  /// this callback is triggered whenever an error on Etoile.
  ///
  Status		Agent::Error(const Report&		report)
  {
    enter();

    // report the error.
    report(Report::TypeError, report);

    // quit the program.
    Program::Exit(StatusError);

    leave();
  }

  ///
  /// this callback is triggered whenever an error occurs on the socket.
  ///
  Status		Agent::Error(const String&		error)
  {
    enter();

    // report the error.
    report(Report::TypeError, error);

    // quit the program.
    Program::Exit(StatusError);

    leave();
  }

//
// ---------- functions -------------------------------------------------------
//

  ///
  /// the main function.
  ///
  Status		Main(const Natural32			argc,
			     const Character*			argv[])
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

    // set up the program.
    if (Program::Setup(argc, argv) == StatusError)
      escape("unable to set up the program");

    // initialize the agent.
    if (Agent::Initialize(name) == StatusError)
      escape("unable to initialize the agent");

    // launch the program.
    if (Program::Launch() == StatusError)
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
int			main(const int				argc,
                             const char*			argv[])
{
  agent::Main(argc, argv);

  expose();

  return (0);
}
