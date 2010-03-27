//
// ---------- header ----------------------------------------------------------
//
// project       8network
//
// license       infinit
//
// file          /home/mycure/infinit/applications/8network/Network.cc
//
// created       julien quintard   [thu mar  4 17:51:46 2010]
// updated       julien quintard   [sat mar 27 11:02:17 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <applications/8network/Network.hh>

namespace application
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const Character	Component[] = "8network";

  ///
  /// this string contains the path to the network infinit configuration
  /// directory.
  ///
  const String		Network::Path = System::Path::Home + "/.infinit";

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method creates a new network by using the user 'name' as the
  /// initial user.
  ///
  Status		Network::New(const String&			name)
  {
    KeyPair			initial;
    etoile::core::Object	directory;

    enter();

    //
    // retrieve the initial user's key pair.
    //
    {
      Archive		archive;
      Region		region;
      struct ::stat	stat;
      String		path = Network::Path + "/keys/" + name + ".pair";
      Integer32		fd;
      String		prompt;
      String		passphrase;
      SecretKey		key;
      Cipher		cipher;

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
      if (key.Decrypt(cipher, initial) == StatusError)
	escape("unable to decrypt the keypair");
    }

    //
    // create a directory.
    //
    {
      Agent		agent(initial);

      // create directory object.
      if (directory.Create(GenreDirectory, agent.pair.K) == StatusError)
	escape("unable to create the object directory");

      // seal the data and meta data.
      if (directory.Seal(agent) == StatusError)
	escape("unable to seal the object");

      // compute the address.
      if (directory.Bind() == StatusError)
	escape("unable to bind the object to an address");
    }

    //
    // store the directory object.
    //
    {
      String		identity;
      Archive		archive;
      Integer32		fd;

      // serialize the directory object.
      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      if (archive.Serialize(directory) == StatusError)
	escape("unable to serialize the directory");

      // store the directory.
      if (directory.address.Identify(identity) == StatusError)
	escape("unable to identify the directory address");

      String		path = Network::Path + "/hole/" + identity;

      // open the file.
      if ((fd = ::open(path.c_str(),
		       O_WRONLY | O_CREAT | O_TRUNC,
		       0600)) == -1)
	escape(::strerror(errno));

      // write the file.
      if (::write(fd, archive.contents, archive.size) != archive.size)
	{
	  ::close(fd);

	  escape("unable to write the directory object");
	}

      // close the file.
      ::close(fd);
    }

    //
    // hack for bootstrapping purposes: store the root directory
    // address in a .root file.
    //
    {
      String		path = Network::Path + "/hole/.root";
      Archive		archive;
      Integer32		fd;

      // create the archive.
      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      // serialize the directory address.
      if (archive.Serialize(directory.address) == StatusError)
	escape("unable to serialize the archive");

      // open the file.
      if ((fd = ::open(path.c_str(),
		       O_WRONLY | O_CREAT | O_TRUNC,
		       0400)) == -1)
	escape(::strerror(errno));

      // write the file.
      if (::write(fd, archive.contents, archive.size) != archive.size)
	{
	  ::close(fd);

	  escape("unable to write the .root file");
	}

      // close the file.
      ::close(fd);
    }

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
    String		command;

    enter();

    /// XXX \todo to improve with real argument parsing.
    {
      if (argc != 3)
	escape("[usage] ./8network {command} {name}");

      command.assign(argv[1]);
      name.assign(argv[2]);
    }

    // initialize the Elle library.
    if (Elle::Initialize() == StatusError)
      escape("unable to initialize the Elle library");

    // trigger a command depending on the arguments.
    if (command == "new")
      Network::New(name);

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
  application::Main(argc, argv);

  expose();

  return (0);
}
