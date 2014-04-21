#include <elle/os/environ.hh>
#include <elle/printf.hh>
#include <elle/system/Process.hh>

using namespace elle::system;

int
main()
{
  using elle::system::Process;
  Process p({elle::sprintf("%s/tests/system/true",
                           elle::os::getenv("BUILD_DIR"))});
  return p.wait();
}
