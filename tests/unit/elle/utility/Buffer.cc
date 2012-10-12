#include <elle/Buffer.hh>

#include <iostream>

int main()
{
  elle::Buffer b;
    {
      int i = 32;
      std::string s = "Bite";
      b.writer() << i << s;
    }
    {
      int i;
      std::string s;
      auto r = b.reader();
      r >> i;
      r >> s;
      assert(i == 32);
      assert(s == "Bite");
    }
  std::cout << "tests done.\n";
  return 0;
}

