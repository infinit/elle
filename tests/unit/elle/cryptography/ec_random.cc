#include <cryptography/random.hh>
#include <elle/Exception.hh>

int
main(int, const char *[])
{
  infinit::cryptography::random::setup();

  std::cout << "tests done." << std::endl;

  return (0);
}
