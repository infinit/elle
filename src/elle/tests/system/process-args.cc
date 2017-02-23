
#include <elle/assert.hh>
#include <elle/system/Process.hh>
#include <elle/Exception.hh>

#include <iostream>

int
main()
{
  using elle::system::Process;

  Process("ls", {"-l", "-a"}).wait();
  return (0);
}
