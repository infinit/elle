#include <elle/os/environ.hh>
#include <elle/printf.hh>
#include <elle/system/Process.hh>

int
main()
{
  using elle::system::Process;
  Process p({elle::sprintf("%s/tests/system/false",
                           elle::os::getenv("BUILD_DIR"))});
  return !p.wait();
}
