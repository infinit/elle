
#include <iostream>

#include <elle/format.hh>

int main()
{
  std::cout << elle::format("Salut %s, vous avez %d de QI", "Jean-Roger", 12) << "\n";
  std::cout << "tests done.\n";
  return 0;
}

