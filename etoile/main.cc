#include <elle/Elle.hh>
#include <infinit/components/Components.hh>

using namespace infinit;
using namespace infinit::components;

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
