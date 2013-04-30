#include <elle/system/Process.hh>
#include <unistd.h>
#include <iostream>

int
main()
{
  using namespace elle::system;

  Process p{"sh", {"-c", "sleep 1 && exit 1"}};
  if (p.wait_status() == 1)
  {
    return 0;
  }
  return 1;
}
