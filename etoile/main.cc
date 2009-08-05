#include <elle/Elle.hh>
#include <etoile/core/Core.hh>

using namespace etoile;
using namespace etoile::core;

//
// RSA[1024] Key Generation:         79.36ms
// Block[4K] RSA[1024] Encryption:    0.29ms
// Block[4K] RSA[1024] Decryption:    7.26ms
// Block[4K] RSA[1024] Signature:     7.40ms
// Block[4K] RSA[1024] Verification:  0.17ms
// Block[4K] Hash:                    0.04ms
// AES[256] Key Generation:           0.08ms
// Block[4K] Encryption AES[256]:     0.10ms
// Block[4K] Decryption AES[256]:     0.14ms
//
// 131 directories, 2089 files, 103 symlinks and 23M of data
//
//			Local		SSHFS		Infinit(w/o)
//
//                                                     Local  Network
//
// Prepare              0.13s           7.11s         14.25s   
// Copy                 7.21s         235.81s        287.81s   
// List                 0.10s           7.45s          1.63s
// Search               0.24s          94.50s          4.84s
// Compile              151.62s       485.86s        520.96s
//
// Optimisations: HMAC, filegroup etc. a citer dans le papier (surtout filegroup)
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

  expose();

  return (0);
}
