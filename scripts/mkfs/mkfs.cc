/*
 * ---------- header ----------------------------------------------------------
 *
 * project       infinit
 *
 * license       infinit (c)
 *
 * file          /home/mycure/infinit/pig/pig.c
 *
 * created       julien quintard   [mon oct 22 12:26:58 2007]
 * updated       julien quintard   [thu jul 23 00:45:36 2009]
 */

/*
 * ---------- includes --------------------------------------------------------
 */

#include <elle/Elle.hh>

#include <etoile/core/Core.hh>
#include <etoile/hole/Hole.hh>

using namespace elle;

using namespace etoile;
using namespace etoile::core;

//
// ---------- globals ---------------------------------------------------------
//

const char*		g_device;

//
// ---------- hole ------------------------------------------------------------
//

int			hole_put(const char*			id,
				 void*				contents,
				 unsigned int			size)
{
  char			path[4096];
  int			fd;

  printf("XXX[%s(%s, 0x%x, %u)]\n",
	 __FUNCTION__, id, contents, size);

  sprintf(path, "%s/%s", g_device, id);

  if ((fd = open(path, O_CREAT | O_WRONLY, 0600)) == -1)
    return (-errno);

  if (write(fd, contents, size) == -1)
    return (-errno);

  if (close(fd) == -1)
    return (-errno);

  return (0);
}

/*
 * ---------- entry point -----------------------------------------------------
 */

void			usage()
{
  printf("usage: mkfs.infinit [path-to-device]\n");
}

int			mkfs(int				argc,
			     char*				argv[])
{
  if (argc != 2)
    {
      usage();
      exit(EXIT_FAILURE);
    }

  g_device = argv[1];

  /*
   * create the device directory.
   */
  {
    mkdir(g_device, 0700);
  }

  /*
   * initialize the libraries.
   */
  {
    Cryptography::Initialize();
  }

  /*
   * create the directory.
   */
  KeyPair		owner;
  Object		directory;
  hole::Address		self;
  {
    // generate owner keypair
    if (owner.Generate() == StatusError)
      return (-1);

    // create directory object.
    if (directory.Create(GenreDirectory, owner.K) == StatusError)
      return (-1);

    // seal the data and meta data.
    if (directory.Seal(owner.k) == StatusError)
      return (-1);

    // get the address.
    if (directory.Self(self) == StatusError)
      return (-1);
  }

  /*
   * store the whole.
   */
  {
    String		identity;

    Archive		a_directory;

    // serialize the directory object.
    if (a_directory.Create() == StatusError)
      return (-1);

    if (a_directory.Serialize(directory) == StatusError)
      return (-1);

    // store the directory.
    if (self.Identify(identity) == StatusError)
      return (-1);

    if (hole_put(identity.c_str(),
		 (void*)a_directory.contents,
		 a_directory.size) != 0)
      return (-errno);
  }

  /*
   * XXX[hack for bootstrapping the system]
   *
   * store the directory into .root
   */
  {
    Archive		a_address;

    // create the archive.
    if (a_address.Create() == StatusError)
      return (-1);

    // serialize the directory address.
    if (a_address.Serialize(self) == StatusError)
      return (-1);

    // store it into a file.
    if (hole_put(".root",
		 (void*)a_address.contents,
		 a_address.size) != 0)
      return (-errno);
  }

  /*
   * XXX[hack for bootstrapping the user]
   *
   * store the user's key pair into .administrator
   */
  {
    Archive		a_keypair;

    // create the archive.
    if (a_keypair.Create() == StatusError)
      return (-1);

    // serialize the owner's keypair.
    if (a_keypair.Serialize(owner) == StatusError)
      return (-1);

    // store the archive.
    if (hole_put(".administrator",
		 (void*)a_keypair.contents,
		 a_keypair.size) != 0)
      return (-errno);
  }

  return (0);
}

int			main(int				argc,
			     char*				argv[])
{
  int			status;

  status = mkfs(argc, argv);

  expose();

  return (status);
}
