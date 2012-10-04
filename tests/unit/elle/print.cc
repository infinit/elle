#include <cassert>

#include <elle/print.hh>

int main()
{
  auto s = elle::sprint("Salut", elle::iomanip::tabsep, "les", elle::iomanip::spacesep, 21, "couillons");
  elle::print((int)s[s.size() - 1]);
  char const* ref = "Salut\tles 21 couillons";
  elle::print((int)ref[s.size() - 1]);
  for (size_t i = 0; i < s.size(); ++i)
    {
      if (s[i] != ref[i])
        {
          std::cerr << i << std::endl;
          assert(false);
        }
    }
  assert(s == ref);
  elle::print("tests", "done.");
  return 0;
}
