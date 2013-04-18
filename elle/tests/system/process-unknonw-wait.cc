#include <elle/system/Process.hh>
#include <unistd.h>
#include <iostream>

int
main()
{
  using namespace elle::system;

  Process p{"TestUnknownProgram"};
  return (p.wait_status() != 0);
}
