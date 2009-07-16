#include <elle/Elle.hh>
#include <etoile/components/Components.hh>

using namespace etoile;
using namespace etoile::components;

int main()
{
  Cryptography::Initialize();

  File file;
  KeyPair owner;

  owner.Generate();

  file.Create(owner);
  file.Seal();

  expose();

  return (0);
}
