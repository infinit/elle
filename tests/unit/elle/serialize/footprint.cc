#include <iostream>
#include <iomanip>
#include <sstream>

#include <elle/serialize/footprint.hh>
#include <elle/cryptography/KeyPair.hh>

int main()
{
  elle::cryptography::KeyPair pair(elle::cryptography::KeyPair::generate());

  elle::Natural32 size = elle::serialize::footprint(pair);

  std::cout << "[size] " << size << std::endl;

  // the key pair footprint should be around 756 bytes.
  assert(size <= 777);

  std::cout << "tests done.\n";
  return 0;
}
