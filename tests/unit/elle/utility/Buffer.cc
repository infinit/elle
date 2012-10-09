#include <elle/utility/Buffer.hh>

#include <iostream>

int main()
{
  elle::utility::Buffer b;
    {
      int i = 32;
      std::string s = "Bite";
      b.Writer() << i << s;
    }
    {
      int i;
      std::string s;
      auto r = b.Reader();
      r >> i;
      r >> s;
      assert(i == 32);
      assert(s == "Bite");
    }
  std::cout << "tests done.\n";
  return 0;
}

