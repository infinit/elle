#include <elle/Elle.hh>
#include <etoile/core/Core.hh>

using namespace etoile;
using namespace etoile::core;

//
// [OPENSSL]
//
// 131 directories, 2089 files, 103 symlinks and 23M of data
//
//			Local(ext3)	NFS3		SSHFS		Infinit(w/o)
//
//                                                                     Local  Network
//
// Prepare              0.13s          2.96s            7.11s         14.25s   
// Copy                 7.21s        201.66s          235.81s        287.81s   
// List                 0.10s         49.08s            7.45s          1.63s
// Search               0.24s        122.67s           94.50s          4.84s
// Compile              151.62s      986.13s          485.86s        520.96s
//
// [KANETON]
//
// 527 directories, 2125 files, 59 symlinks and 18M of data
//
//                   Local(ext3)      NFS3            SSHFS             Infinit(w/o)
//
//                                                                     Local  Network
//
// Prepare              0.56s         12.75s           29.96s         56.52s
// Copy                 1.99s        210.19s          268.63s        292.60s
// List                 0.05s         71.61s           30.64s          2.26s
// Search               0.06s        143.85s          105.06s          5.84s
// Compile             19.97s        511.96s         1289.12s        170.92s
//

//
// Optimisations: filegroup etc. a citer dans le papier (surtout filegroup bien
//                que dans notre cas, ca ne va pas trop trop optimiser.
//

int		main(int			argc,
		     char**			argv)
{
  Cryptography::Initialize();

  Archive	archive;
  Digest	digest;

  archive.Create();

  OneWay::Hash(archive, digest);

  KeyPair	keypair;
  Signature	signature;

  keypair.Generate();

  keypair.k.Sign(keypair, digest, keypair, signature);

  File		file;

  PublicKeyBlock	pkb;

  pkb.Create();
  pkb.Seal();

  file.Create(keypair);
  file.Seal();

  String	identifier;

  file.address.Identify(identifier);

  std::cout << file.address.digest->region << std::endl;
  std::cout << identifier << std::endl;

  Archive	ar;

  ar.Create();
  ar.Serialize(file);

  printf("%u -> %u\n", sizeof(file), ar.size);

  expose();

  return (0);
}
