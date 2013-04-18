#include <elle/assert.hh>
#include <elle/system/Process.hh>
#include <elle/Exception.hh>

int
main()
{
  using elle::system::Process;

  try
  {
    Process("ls", {"JE N'ÉXISTE PAS"}).wait();
  }
  catch (elle::Exception const& e)
  {
    return 0;
  }
  catch (...)
  {
    return 1;
  }
  return 1;
}
