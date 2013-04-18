#include <elle/system/Process.hh>
#include <unistd.h>
#include <iostream>

int
main()
{
  using namespace elle::system;

  Process p{"sh", {"-c", "sleep 10"}};
  if (!(p.status() == 0))
  {
    return 1;
  }
  p.kill();
  if (!(p.status() != 0))
  {
    return 1;
  }
  return 0;
}
