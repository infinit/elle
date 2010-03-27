//
// ---------- header ----------------------------------------------------------
//
// project       8user
//
// license       infinit
//
// file          /home/mycure/infinit/applications/8user/User.cc
//
// created       julien quintard   [thu mar  4 17:51:46 2010]
// updated       julien quintard   [sat mar 27 09:57:01 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <applications/8user/User.hh>

namespace application
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const Character	Component[] = "8user";

  ///
  /// this string contains the path to the user infinit configuration
  /// directory.
  ///
  const String		User::Path = System::Path::Home + "/.infinit";

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method creates a new user by generating a new key pair.
  ///
  Status		User::New(const String&			name)
  {
    Archive		archive;
    struct ::stat	stat;
    String		path = User::Path + "/keys/" + name + ".pair";
    Integer32		fd;
    String		prompt;
    String		passphrase;
    SecretKey		key;
    Cipher		cipher;
    KeyPair		pair;

    enter();

    // get the file status.
    if (::stat(path.c_str(), &stat) == 0)
      escape("this user seems to already exist");

    // generate a key pair.
    if (pair.Generate() == StatusError)
      escape("unable to generate the key pair");

    // prompt the user for the passphrase.
    prompt = "Enter passphrase for keypair '" + path + "': ";
    passphrase = String(::getpass(prompt.c_str()));

    // create a secret key with this passphrase.
    if (key.Create(passphrase) == StatusError)
      escape("unable to create the secret key");

    // encrypt the keypair.
    if (key.Encrypt(pair, cipher) == StatusError)
      escape("unable to decrypt the keypair");

    // create an archive.
    if (archive.Create() == StatusError)
      escape("unable to create the archive");

    // serialize the encrypted key pair.
    if (archive.Serialize(cipher) == StatusError)
      escape("unable to serialize the cipher");

    // open the file.
    if ((fd = ::open(path.c_str(),
		     O_WRONLY | O_CREAT | O_TRUNC,
		     0400)) == -1)
      escape(::strerror(errno));

    // write the file.
    if (::write(fd, archive.contents, archive.size) != archive.size)
      {
	::close(fd);

	escape("unable to write the key pair file");
      }

    // close the file.
    ::close(fd);

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
	escape("[usage] ./8user {command} {name}");

      command.assign(argv[1]);
      name.assign(argv[2]);
    }

    // initialize the Elle library.
    if (Elle::Initialize() == StatusError)
      escape("unable to initialize the Elle library");

    // trigger a command depending on the arguments.
    if (command == "new")
      User::New(name);

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
