#include <elle/system/Process.hh>

#include <iostream>

#include <sys/types.h>
#include <unistd.h>

int
main()
{
  using namespace elle::system;

  try
  {
    Process p{"TestUnknownProgram"};
    p.wait();
    return 1;
  }
  catch (std::runtime_error const& err)
  {
    std::cerr << "got expected error: " << err.what() << std::endl;
  }
  return 0;
}
