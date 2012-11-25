#include <elle/types.hh>
#include <elle/assert.hh>
#include <elle/finally.hh>

#include <iostream>

int main()
{
  void* ptr1 = malloc(10);

  ELLE_FINALLY_ACTION_FREE(ptr1);

  elle::Finally guard2([] () { std::cout << "lambda" << std::endl; });

  std::cout << "tests done." << std::endl;

  return 0;
}
