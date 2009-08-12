#include <elle/Elle.hh>

#include <etoile/core/Core.hh>
#include <etoile/path/Path.hh>

using namespace etoile;
using namespace etoile::core;
using namespace etoile::path;

//
// [OPENSSL]
//
// 131 directories, 2089 files, 103 symlinks and 23M of data
//
//			Local(ext3)	NFS3		SSHFS		Infinit (local w/o)
//
// Prepare              0.13s          2.96s            7.11s                  14.25s
// Copy                 7.21s        201.66s          235.81s                 287.81s
// List                 0.10s         49.08s            7.45s                   1.63s
// Search               0.24s        122.67s           94.50s                   4.84s
// Compile              151.62s      986.13s          485.86s                 520.96s
//
// [KANETON]
//
// 527 directories, 2125 files, 59 symlinks and 18M of data
//
//                   Local(ext3)      NFS3            SSHFS     Infinit (local w/o)   Infinit (NFS3 w/o)   Infinit(SSHFS w/o)
//
// Prepare              0.56s         12.75s           29.96s         56.52s              560.57s               615.33s
// Copy                 1.99s        210.19s          268.63s        292.60s             5719.40s              6110.99s
// List                 0.05s         71.61s           30.64s          2.26s             1581.46s              1645.30s
// Search               0.06s        143.85s          105.06s          5.84s             3136.82s              3185.92s
// Compile             19.97s        511.96s         1289.12s        170.92s          too long... 24h+      too long... 24h+
//
// Optimisations: paths cache, blocks 2-level cache (main memory + local) + OKB + try to turn debugging off
//

//
// Optimisations: filegroup etc. a citer dans le papier (surtout filegroup bien
//                que dans notre cas, ca ne va pas trop trop optimiser.
//

int		main(int			argc,
		     char**			argv)
{
  Cryptography::Initialize();

  Address	root;

      // XXX[hack for the /]
      {
	int		fd;

	fd = ::open(".device/.root", O_RDONLY);

	Region	region;

	region.Prepare(4096);

	region.size = read(fd, region.contents, region.capacity);

	region.Detach();

	Archive		archive;

	archive.Prepare(region);

	archive.Extract(root);

	expose();
      }

  Path::Initialize(root);

  Route		route;

  route.Create("/suce/mon");

  Address	address;

  Path::Resolve(route, address);

  address.Dump();

  expose();

  return (0);
}
