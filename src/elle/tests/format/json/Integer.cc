#include <cassert>
#include <iostream>

#include <elle/format/json/Integer.hh>

namespace json = elle::format::json;

int main()
{
  json::Integer i;

  assert(i == 0);
    {
      auto clone_ptr = i.clone();
      auto casted = static_cast<json::Integer*>(clone_ptr.get());
      assert(casted != nullptr);
      assert(*casted == 0);
      assert(casted->repr() == "0");
    }
  assert(i.repr() == "0");


  i = 2;

  assert(i == 2);
    {
      auto clone_ptr = i.clone();
      auto casted = static_cast<json::Integer*>(clone_ptr.get());
      assert(*casted == 2);
      assert(casted->repr() == "2");
    }
  assert(i.repr() == "2");

  std::cout << "tests done.\n";
  return 0;
}
