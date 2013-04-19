#include <elle/system/Process.hh>
#include <elle/Exception.hh>
#include <unistd.h>
#include <iostream>

int
main()
{
  using namespace elle::system;

  try
  {
    Process p{"TestUnknownProgram"};

    return (!(p.wait_status() != 0));
  }
  catch (elle::Exception const& e)
  {
    return 1;
  }
  return 1;
}
