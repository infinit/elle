#include <cassert>
#include <iostream>
#include <sstream>

#include "elle/serialize/Base64Archive.hh"
#include "elle/serialize/BinaryArchive.hh"
#include "elle/serialize/JSONArchive.hxx"

using namespace elle::serialize;

template<template <ArchiveMode> class Archive>
void testLinear()
{
  std::stringstream ss;
    {
      Archive<ArchiveMode::Output> ar(ss);
      ar << named("pif", 12);
      ar << named("paf", std::string("Salut"));
      ar << named("a float", 12.5f);
      ar << 42.01;
      ar << named("wqw, ", 'c');
      ar << '1';
    }

    {
      Archive<ArchiveMode::Input> ar(ss);
      int int_;
      std::string string_;
      float float_;
      double double_;
      char char_;

      ar >> named("pif", int_);
      assert(int_ == 12);
      ar >> named("paf", string_);
      assert(string_ == std::string("Salut"));
      ar >> named("a float", float_);
      assert(float_ == 12.5f);
      ar >> double_;
      assert(double_ == 42.01);
      ar >> char_;
      assert(char_ == 'c');
      ar >> char_;
      assert(char_ == '1');
    }
}

struct A
{
  std::string a;
  std::string b;
  std::string c;
  float       float_;

  bool operator ==(A const& o) const
    {
      return (
              a == o.a
          &&  b == o.b
          &&  c == o.c
          &&  float_ == o.float_
      );
    }
};

namespace elle { namespace serialize {
  template<> struct ArchiveSerializer<A>
  {
    template<typename Archive>
    static void Serialize(Archive& ar, A& a, unsigned int)
      {
        ar & named("a", a.a);
        ar & named("b", a.b);
        ar & named("c", a.c);
        ar & named("a float", a.float_);
      }
  };
}}

template<template <ArchiveMode> class Archive> void testClass()
{
  std::stringstream ss;
    {
      Archive<ArchiveMode::Output> ar(ss);
      ar & named("el1", A{"salut", "pif", "paf", 42.3f});
      ar << named("el2", A{"1", "2", "3", 77.5f});
    }
    {
      Archive<ArchiveMode::Input> ar(ss);
      A a;
      ar & named("el1", a);
      assert(a.a == "salut");
      assert(a.b == "pif");
      assert(a.c == "paf");
      assert(a.float_ == 42.3f);
      ar & named("el2", a);
      assert(a.a == "1");
      assert(a.b == "2");
      assert(a.c == "3");
      assert(a.float_ == 77.5f);
    }
}

template<typename T> void testJSON()
{
  std::stringstream ss;
    {
      OutputJSONArchive ar(ss, T{"1", "2", "3", 12.3});
    }
    {
      T t;
      InputJSONArchive ar(ss, t);
      assert((t == T{"1", "2", "3", 12.3}));
    }
}



int main()
{
  testLinear<BinaryArchive>();
  testLinear<Base64Archive>();

  testClass<BinaryArchive>();
  testClass<Base64Archive>();

  testJSON<A>();

  std::cout << "tests done.\n";
  return 0;
}



