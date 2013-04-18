#include <elle/system/Process.hh>

using namespace elle::system;

int
main()
{
  using elle::system::Process;

  Process("ls", {}).wait();
  return (0);
}
