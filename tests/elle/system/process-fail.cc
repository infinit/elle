#include <elle/os/environ.hh>
#include <elle/printf.hh>
#include <elle/system/Process.hh>

#ifndef INFINIT_WINDOWS
# define EXTENSION ""
#else
# define EXTENSION ".exe"
#endif

int
main()
{
  using elle::system::Process;
  Process p({elle::sprintf("%s/system/false" EXTENSION,
                           elle::os::getenv("TEST_DIR"))});
  return !p.wait();
}
