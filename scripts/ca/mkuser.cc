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

using namespace elle;

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
  printf("usage: mkuser [device] [username]\n");
}

int			mkuser(int				argc,
			       char*				argv[])
{
  if (argc != 3)
    {
      usage();
      exit(EXIT_FAILURE);
    }

  g_device = argv[1];

  /*
   * initialize the libraries.
   */
  {
    Cryptography::Initialize();
  }

  /*
   * create the keypair.
   */
  KeyPair		owner;
  {
    // generate owner keypair
    if (owner.Generate() == StatusError)
      return (-1);
  }

  /*
   * store the keypair.
   */
  {
    Archive		archive;

    char		name[128];

    // serialize the directory object.
    if (archive.Create() == StatusError)
      return (-1);

    if (archive.Serialize(owner) == StatusError)
      return (-1);

    // compute the name.
    sprintf(name, ".%s", argv[2]);

    // store the keypair.
    if (hole_put(name,
		 (void*)archive.contents,
		 archive.size) != 0)
      return (-errno);
  }

  return (0);
}

int			main(int				argc,
			     char*				argv[])
{
  int			status;

  status = mkuser(argc, argv);

  expose();

  return (status);
}
