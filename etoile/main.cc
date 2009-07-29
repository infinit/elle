#include <elle/Elle.hh>
#include <etoile/core/Core.hh>

using namespace etoile;
using namespace etoile::core;

//
// RSA[1024] Key Generation:         67.25ms
// Block[4K] RSA[1024] Encryption:    0.19ms
// Block[4K] RSA[1024] Decryption:    5.90ms
// Block[4K] RSA[1024] Signature:     5.81ms
// Block[4K] RSA[1024] Verification:  0.13ms
// Block[4K] Hash:                    0.003ms
// AES[256] Key Generation:           0.008ms
// Block[4K] Encryption AES[256]:     0.009ms
// Block[4K] Decryption AES[256]:     0.008ms
//
// 690 directories, 
//
//			Local		Centralised		Decentralised
//
//                                                              Proc      Net
//
// Prepare              0.13s               7.11s              23.10s   24.70s
// Copy                 7.21s             235.81s             385.71s   
// List                 0.10s               7.45s               0.91s
// Search               0.24s              94.50s               2.09s
// Compile              151.62s          7485.86s             332.03s
//
// Optimisations: HMAC, filegroup etc.
//

int		main(int			argc,
		     char**			argv)
{
  expose();

  return (0);
}
