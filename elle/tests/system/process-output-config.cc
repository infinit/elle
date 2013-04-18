#include <elle/system/Process.hh>
#include <unistd.h>
#include <iostream>

int
main()
{
  using namespace elle::system;

  Process p{check_output_config, "echo", {"toto"}};
  if (p.read() == "toto\n")
  {
      p.wait();
      return 0;
  }
  p.wait();
  return 1;
}
