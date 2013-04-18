#include <elle/assert.hh>
#include <elle/system/Process.hh>

#include <iostream>

int main()
{
  using elle::system::Process;
  Process("ls", {}).wait();
  Process("ls", {"-l", "-a"}).wait();

  ELLE_ASSERT(Process("ls", {"JE N'EXISTE PAS"}).wait_status() > 0);
  try
    {
      auto p = Process("ls", {"JE N'EXISTE PAS"});
      ELLE_ASSERT(p.wait_status() > 0);
      p.wait();
      ELLE_ASSERT(false && "Should not pass");
    }
  catch (...) {}

  try
    {
      Process("ls", {"JE N'EXISTE PAS"}).wait();
      ELLE_ASSERT(false && "Should not pass");
    }
  catch (...) {}

  std::cout << "tests done.\n";
  return 0;
}

