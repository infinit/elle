#include <elle/serialize/footprint.hh>

#include <cryptography/KeyPair.hh>
// XXX[temporary: for cryptography]
using namespace infinit;

#include <iostream>
#include <iomanip>
#include <sstream>

int main()
{
  cryptography::KeyPair pair =
    cryptography::KeyPair::generate(cryptography::Cryptosystem::rsa, 512);

  elle::Natural32 size = elle::serialize::footprint(pair);

  std::cout << "[size] " << size << std::endl;

  // the key pair footprint should be around 756 bytes.
  assert(size <= 777);

  std::cout << "tests done.\n";
  return 0;
}
