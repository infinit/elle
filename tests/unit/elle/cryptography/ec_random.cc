#include <cryptography/random.hh>

#include <elle/types.hh>
#include <elle/Exception.hh>

int
main(int, const char *[])
{
  infinit::cryptography::random::setup();

  elle::Natural32 n =
    infinit::cryptography::random::generate<elle::Natural32>(3, 15);

  std::cout << "tests done." << std::endl;

  return (0);
}
