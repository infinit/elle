#include <cassert>
#include <iostream>

#include "elle/format/json/Dictionary.hxx"

namespace json = elle::format::json;

static void test_simple()
{
  json::Dictionary d;

  assert(d.size() == 0);

  d["pif"] = 12;
  assert(d["pif"] == 12);
  assert(!(d["paf"] == 14));
  d["paf"] = 14;
  assert(d["paf"] == 14);
  d["paf"] = 15;
  assert(d["paf"] == 15);
    {
      auto o = d["pouf"];
      o = 12;
    }
  assert(d["pouf"] == 12);

  d["pif"] = true;
  assert(d["pif"].repr() == "true");
  assert(d["pif"] == true);
  assert(d["pif"] != false);

  d["string"] = "Hello unit test";

  assert(d["string"] == "Hello unit test");
  assert(d["string"] != "Coucou");
  d["inception"] = d;

  assert(dynamic_cast<json::Dictionary const*>(d.contains("inception")) != nullptr);
}

int main()
{
  std::cout << "tests done.\n";
  test_simple();
  return 0;
}


