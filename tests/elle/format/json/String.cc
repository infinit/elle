
#include <cassert>
#include <iostream>

#include "elle/format/json/String.hh"

namespace json = elle::format::json;

int main()
{
  json::String value;

  assert(value == "");
    {
      auto clone_ptr = value.Clone();
      auto casted = dynamic_cast<json::String*>(clone_ptr.get());
      assert(casted != nullptr);
      assert(*casted == "");
      assert(casted->repr() == "\"\"");
    }
  assert(value.repr() == "\"\"");


  value = "Coucou";

  assert(value == "Coucou");
    {
      auto clone_ptr = value.Clone();
      auto casted = static_cast<json::String*>(clone_ptr.get());
      assert(*casted == "Coucou");
      assert(casted->repr() == "\"Coucou\"");
    }
  assert(value.repr() == "\"Coucou\"");

  std::cout << "tests done.\n";
  return 0;
}

