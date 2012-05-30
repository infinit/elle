#include <cassert>

#include <elle/print.hh>


int main()
{
  auto s = elle::sprint("Salut", elle::iomanip::tabsep, "les", elle::iomanip::spacesep, 21, "couillons");
  elle::print(s);
  assert(s == "Salut\tles 21 couillons");
  elle::print("tests", "done.");
  return 0;
}
