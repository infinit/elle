#include <cassert>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
#include <list>

#include <elle/format/json/Dictionary.hh>
#include <elle/format/json/Array.hh>

namespace json = elle::format::json;

static void test_simple()
{
  json::Dictionary d;

  assert(d.size() == 0);

  d["pif"] = 12;
  assert(d["pif"] == 12);
  try
    {
      if (d["paf"] == 14)
        assert(false && "Wrong result!");
      assert(false && "Should have thrown !");
    }
  catch (json::Dictionary::KeyError const&)
    {
      // it's ok :)
    }
  catch (...)
    {
      assert(false && "Should have been cought before !");
    }

  short fifteen = 15;
  d["paf"] = fifteen;
  assert(d["paf"] == 15);
  d["paf"] = 14;
  assert(d["paf"] == 14);
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


static
void
test_stl()
{
  // construct from std::map
    {
      std::map<std::string, int> m;
      json::Dictionary d{m};
      assert(d.size() == 0);
    }
    {
      std::map<std::string, int> m{{"pif", 12}};
      json::Dictionary d{m};
      assert(d.size() == 1);
      assert(d["pif"] == 12);
      assert(d.repr() == "{\"pif\":12}");
    }

  // construct from std::unordered_map
    {
      std::unordered_map<std::string, int> m;
      json::Dictionary d{m};
      assert(d.size() == 0);
    }
    {
      std::unordered_map<std::string, int> m{{"pif", 12}};
      json::Dictionary d{m};
      assert(d.size() == 1);
      assert(d["pif"] == 12);
      assert(d.repr() == "{\"pif\":12}");
    }

  // insert vector
    {
      json::Dictionary d;
      std::vector<int> v = {1,2,3,4,5};
      d["pif"] = v;
      assert(d.repr() == "{\"pif\":[1,2,3,4,5]}");
    }

  // insert list
    {
      json::Dictionary d;
      std::list<int> v = {1,2,3,4,5};
      d["pif"] = v;
      assert(d.repr() == "{\"pif\":[1,2,3,4,5]}");
    }

  // insert raw array
    {
      json::Dictionary d;
      int v[] = {1,2,3,4,5};
      d["pif"] = v;
      assert(d.repr() == "{\"pif\":[1,2,3,4,5]}");
    }

  // insert std::map
    {
      std::unordered_map<std::string, int> m{{"pif", 12}};
      json::Dictionary d;
      d["paf"] = m;
      assert(d.repr() == "{\"paf\":{\"pif\":12}}");
    }

}

static
void
test_except()
{
  json::Dictionary d;

  try
    {
      if (d["paf"] == 14)
        assert(false && "Wrong result!");
      assert(false && "Should have thrown !");
    }
  catch (json::Dictionary::KeyError const&)
    {
      // it's ok :)
    }
  catch (...)
    {
      assert(false && "Should have been cought before !");
    }

  try
    {
      if (d["paf"] != 14)
        assert(false && "Wrong result!");
      assert(false && "Should have thrown !");
    }
  catch (json::Dictionary::KeyError const&)
    {
      // it's ok :)
    }
  catch (...)
    {
      assert(false && "Should have been cought before !");
    }

  d["pif"] = 12;
  auto o = d["paf"]; // o is a dict proxy

  try
    {
      json::Object const& o = d["paf"];
      assert(false && "Should have thrown !");
      (void) o; // no unused var
    }
  catch (json::Dictionary::KeyError const&)
    {
      // it's ok :)
    }
  catch (...)
    {
      assert(false && "Should have been cought before !");
    }

  o = 14;
  assert(d["paf"] == 14);
}

int main()
{
  test_simple();
  test_stl();
  test_except();
  std::cout << "tests done.\n";
  return 0;
}
