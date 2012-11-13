#include <elle/types.hh>
#include <elle/assert.hh>
#include <elle/finally.hh>

#include <iostream>

void _free(void* ptr)
{
  std::cout << "freeing" << std::endl;
  free(ptr);
}

int main()
{
  void* ptr = malloc(10);

  ELLE_FINALLY_FREE(guard1, ptr);

  ELLE_FINALLY_LAMBDA(guard2, [] () { std::cout << "lambda" << std::endl; });
  guard2.abort();

  std::cout << "tests done." << std::endl;

  return 0;
}
