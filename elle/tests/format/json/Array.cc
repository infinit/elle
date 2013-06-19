
#include <cassert>
#include <iostream>
#include <list>
#include <set>

#include <elle/format/json/Array.hh>
#include <elle/format/json/Null.hh>
#include <elle/format/json/Bool.hh>

namespace json = elle::format::json;

static void test_simple()
{
  json::Array arr;

  assert(arr.size() == 0);
  assert(arr.repr() == "[]");

  arr.push_back(10);
  assert(arr.size() == 1);
  assert(arr.repr() == "[10]");

  arr.push_back(arr);
  assert(arr.size() == 2);
  assert(arr.repr() == "[10,[10]]");

  arr.push_back(42.3);
  assert(arr.size() == 3);
  assert(arr.repr() == "[10,[10],42.3]");

  int tab[] = {1,2,3,4,5};
  arr.push_back(tab);
  //
  //  This should not compile
  //
  // int *ptr = &tab[0];
  // arr.push_back(ptr);
  //
  assert(arr.size() == 4);
  assert(arr.repr() == "[10,[10],42.3,[1,2,3,4,5]]");

  arr.push_back(json::null);
  arr.push_back(json::null);
  arr.push_back(json::null);
  arr.push_back(json::null);
  assert(arr.size() == 8);
  assert(arr.repr() == "[10,[10],42.3,[1,2,3,4,5],null,null,null,null]");

  arr.push_back(json::true_);
  arr.push_back(json::false_);
  arr.push_back(true);
  arr.push_back(false);
  assert(arr.size() == 12);
  assert(arr.repr() == "[10,[10],42.3,[1,2,3,4,5],null,null,null,null,true,false,true,false]");
}

static
void
test_stl()
{
    {
      json::Array arr;
      std::vector<int> tab({1,2,3,4});
      arr.push_back(tab);
      assert(arr.size() == 1);
      assert(arr.repr() == "[[1,2,3,4]]");
    }

    {
      std::vector<int> tab({1,2,3,4});
      json::Array arr{tab};
      assert(arr.size() == 4);
      assert(arr.repr() == "[1,2,3,4]");
    }

    {
      json::Array arr;
      std::list<int> tab({1,2,3,4});
      arr.push_back(tab);
      assert(arr.size() == 1);
      assert(arr.repr() == "[[1,2,3,4]]");
    }

    {
      std::list<int> tab({1,2,3,4});
      json::Array arr{tab};
      assert(arr.size() == 4);
      assert(arr.repr() == "[1,2,3,4]");
    }

    {
      json::Array arr;
      int tab[] = {1,2,3,4};
      std::set<int> set{tab, tab + 4};
      arr.push_back(set);
      assert(arr.size() == 1);
      assert(arr.repr() == "[[1,2,3,4]]");
    }

    {
      int tab[] = {1,2,3,4};
      std::set<int> set{tab, tab + 4};
      json::Array arr{set};
      assert(arr.size() == 4);
      assert(arr.repr() == "[1,2,3,4]");
    }
}

int main()
{
  test_simple();
  test_stl();
  std::cout << "tests done.\n";
  return 0;
}
