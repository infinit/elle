#include <iostream>
#include <iomanip>
#include <sstream>

#include <elle/Elle.hh>
#include <elle/serialize/footprint.hh>
#include <elle/cryptography/KeyPair.hh>

int main()
{
  elle::cryptography::KeyPair pair;

  if (elle::Elle::Initialize() == elle::Status::Error)
    assert(false);

  if (pair.Generate(1024) == elle::Status::Error)
    assert(false);

  elle::Natural32 size = elle::serialize::footprint(pair);

  if (elle::Elle::Clean() == elle::Status::Error)
    assert(false);

  std::cout << "[size] " << size << std::endl;

  // XXX assert(size == XXX);

  std::cout << "tests done.\n";
  return 0;
}
