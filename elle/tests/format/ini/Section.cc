
#include <cassert>
#include <iostream>

#include <elle/format/ini/Section.hh>

namespace ini = elle::format::ini;

int main()
{
  ini::Section section("test");
  section.Set("bite", 12);
  assert(section.Contains("bite"));
  assert(section.Get<int>("bite") == 12);
  assert(section.Get<short>("bite") == 12);

  section.Set("bite", 42);
  assert(section.Contains("bite"));
  assert(*section.Contains("bite") == "42");
  assert(section.Get<int>("bite") == 42);
  assert(section.Get<short>("bite") == 42);

  std::cout << "tests done.\n";
  return 0;
}

