#include <elle/system/Process.hh>
#include <unistd.h>
#include <iostream>

int
main()
{
  using namespace elle::system;

  try
  {
    std::cout << check_output("cat", "/etc/passwd") ;
    std::cout << check_output("cat", "-e", "/etc/passwd") ;
    std::cout << check_output("sh", "-c", "env") ;
    std::cout << check_output("env") ;
  }
  catch (...)
  {
    return 1;
  }

  return 0;
}
