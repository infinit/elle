#include <cassert>

#include <elle/print.hh>


int main()
{
  auto s = elle::stringify("Salut", elle::iomanip::tabsep, "les", elle::iomanip::spacesep, 21, "couillons");
  elle::print(s);
  assert(s == "Salut\tles 21 couillons");
  return 0;
}
