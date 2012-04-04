
#include <cassert>
#include <iostream>

#include "elle/format/json/Bool.hh"

namespace json = elle::format::json;

int main()
{
  json::Bool value;

  assert(value == false);
    {
      auto clone_ptr = value.Clone();
      auto casted = dynamic_cast<json::Bool*>(clone_ptr.get());
      assert(casted != nullptr);
      assert(*casted == false);
      assert(casted->repr() == "false");
    }
  assert(value.repr() == "false");


  value = true;

  assert(value == true);
    {
      auto clone_ptr = value.Clone();
      auto casted = static_cast<json::Bool*>(clone_ptr.get());
      assert(*casted == true);
      assert(casted->repr() == "true");
    }
  assert(value.repr() == "true");

  std::cout << "tests done.\n";
  return 0;
}
