#include <elle/Elle.hh>

#include <etoile/core/Core.hh>
#include <etoile/path/Path.hh>
#include <etoile/wall/Wall.hh>
#include <etoile/agent/Agent.hh>

#include <etoile/context/Context.hh>

#include <QCoreApplication>

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
  ::QCoreApplication	app(argc, argv);

  elle::crypto::Cryptography::Initialize();
  etoile::core::Core::Initialize();

  {
    KeyPair	pair;

    etoile::hole::Address	root;

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

      std::cout << "[root directory]" << std::endl;
      root.Dump();
    }

    // XXX[hack for the administrator]
    {
      int		fd;

      fd = ::open(".device/.administrator", O_RDONLY);

      Region	region;

      region.Prepare(4096);

      region.size = read(fd, region.contents, region.capacity);

      region.Detach();

      Archive		archive;

      archive.Prepare(region);

      archive.Extract(pair);

      expose();

      std::cout << "[administrator key pair]" << std::endl;
      pair.Dump();
    }

    {
      etoile::depot::Depot::Initialize();

      etoile::core::Object* o = new etoile::core::Object;

      o->Create(etoile::core::GenreFile, pair.K);
      o->Seal(pair.k);
      o->Bind();

      etoile::depot::Depot::Put(o->address, o);

      etoile::core::Data* data = new etoile::core::Data;
      data->Write(0, (Byte*)"suce", 4);
      data->Bind();

      etoile::depot::Depot::Put(data->address, data);

      etoile::core::Object* n;

      if (etoile::depot::Depot::Get(o->address, n) != StatusOk)
	{
	  printf("... could not get the block\n");
	  return (0);
	}

      etoile::depot::Repository::Dump();

      expose();

      // QT loop
      app.exec();
    }

    /* XXX
    // init the agent.
    Agent::Pair = pair;
    Agent::Subject.Create(Agent::Pair.K);

    // init path.
    Path::Initialize(root);

    {
      context::Directory	context;

      components::Directory::Load(context, root);
      components::Directory::Add(context, "loop", context.address);
      components::Directory::Commit(context);
      //components::Directory::Close(context);
    }
    */

    expose();
  }

  Cryptography::Clean();

  return (0);
}
