#include <elle/Elle.hh>
#include <core/components/Components.hh>

using namespace core;
using namespace core::components;

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
